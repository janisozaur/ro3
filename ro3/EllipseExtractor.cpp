#include "EllipseExtractor.h"

#include <qmath.h>
#include <QElapsedTimer>
#include <QAtomicInt>
#include <QVector>

#include <QDebug>

#define ABS(x) ((x) < 0 ? -(x) : (x))

EllipseExtractor::EllipseExtractor()
{
}

QList<Ellipse> EllipseExtractor::extract(const QImage &image) const
{
	Q_ASSERT(image.format() == QImage::Format_Indexed8);
	const int minDist = 35;
	const int twoMinDist = 2 * minDist;
	const int minDistSq = minDist * minDist;
	const int twoMinDistSq = twoMinDist * twoMinDist;
	const int minThresh = 25;
	const int minVotes = 30;
	QList<Ellipse> result;
	const int w = image.width();
	const int h = image.height();
	QVector<QPoint> points;
	for (int y = 0; y < h; y++) {
		const uchar *d = image.constScanLine(y);
		for (int x = 0; x < w; x++) {
			if (d[x] >= minThresh) {
				points.append(QPoint(x, y));
			}
		}
	}
	const int pointsCount = points.size();
	const int pixelCount = w * h;
	QVector<QAtomicInt> r(pixelCount);
	QAtomicInt *rPtr = r.data();
	int count = 0;
	QAtomicInt progress = 0;
	// (3)
	for (int i = 0; i < pointsCount; i++) {
		QElapsedTimer et;
		et.start();
		const QPoint &p1 = points.at(i);
		const int x1 = p1.x();
		const int y1 = p1.y();
		// (4)
#pragma omp parallel for schedule(guided)
		for (int j = i + 1; j < pointsCount; j++) {
			const QPoint &p2 = points.at(j);
			const int x2 = p2.x();
			const int y2 = p2.y();
			const int dy = y2 - y1;
			const int dx = x2 - x1;
			const int distSq = dx * dx + dy * dy;
			if (distSq < twoMinDistSq) {
				continue;
			}
			const qreal dist = qSqrt(distSq);
			// (5)
			const qreal x0 = qreal(x1 + x2) / 2;
			const qreal y0 = qreal(y1 + y2) / 2;
			const qreal major = dist / 2;
			const qreal majorSq = major * major;
			const qreal orientation = atan2(qreal(dy), qreal(dx));
			QHash<long long, int> votes;
			// (6)
			for (int k = 0; k < pointsCount; k++) {
				if (Q_UNLIKELY(k == i || k == j)) {
					continue;
				}
				const QPoint &p = points.at(k);
				const int x = p.x();
				const int y = p.y();
				const int dyi = y0 - y;
				const int dxi = x0 - x;
				const qreal dSq = dxi * dxi + dyi * dyi;
				// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
				// NOTE: this here might also include "d > major"
				// condition, for we don't want any point that is
				// further than the tip of major axis
				// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
				if (dSq < minDistSq) {
					continue;
				}
				const qreal d = qSqrt(dSq);
				// (7)
				const int fy = y2 - y;
				const int fx = x2 - x;
				const qreal fSq = fy * fy + fx * fx;
				const qreal cosTau = (majorSq + dSq - fSq) / (0.00001 + 2 * major * d);
				const qreal cosTauSq = cosTau * cosTau;
				const qreal sinTauSq = 1 - cosTauSq;
				const qreal nom = majorSq * dSq * sinTauSq;
				const qreal den = majorSq - dSq * cosTauSq + 0.00001;
				// (8)
				const qreal minorSq = nom / den;
				long long iMinorSq = qMax(minorSq, qreal(0));
				if (iMinorSq > 0) {
					votes[iMinorSq]++;
				}
			}
			int i;
			int maxVote = 0;
			int maxVoteIdx = 0;
			for (i = 0; i < votes.size(); i++) {
				if (votes.values().at(i) > maxVote) {
					maxVote = votes.values().at(i);
					maxVoteIdx = i;
				}
			}
			count++;
			if ((maxVote) > minVotes) {
				qDebug() << "ellipse found!" << maxVote << votes.keys().at(maxVoteIdx)
						 << "c" << x0 << y0 << "p1" << x1 << y1
						 << "a" << major << qSqrt(votes.keys().at(maxVoteIdx))
						 << "o" << orientation << "cnt" << count;
			} else {
				qDebug() << "boohoo, no ellipse for me... max vote:" << maxVote << x0 << y0 << votes.keys().at(maxVoteIdx);
			}
			rPtr[int(y0 * w + x0)].fetchAndAddRelaxed(1);
		}
		const int msecs = et.elapsed();
		const int progVal = progress.fetchAndAddRelaxed(1) + 1;
		qDebug() << "msecs:" << msecs << ", " << qreal(progVal) / qreal(pointsCount) * 100 << "%";
	}
	int maxR = 0;
	int maxIdx = 0;
	for (int i = 0; i < r.size(); i++) {
		if (r.at(i) > maxR) {
			maxR = r.at(i);
			maxIdx = i;
		}
	}
	qDebug() << "max" << maxR << maxIdx << maxIdx / w << maxIdx % w;
	const qreal scale = qreal(255) / qreal(maxR);
	QImage rmap(image.size(), QImage::Format_ARGB32);
	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			const int c = r.at(y * w + x) * scale;
			rmap.setPixel(x, y, qRgb(c, c, c));
		}
	}
	rmap.save("vote_map.png");
	return result;
}
