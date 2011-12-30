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

QList<Ellipse> EllipseExtractor::extract(const QImage &image, const QImage &org, const int &lower, const int &upper) const
{
	Q_ASSERT(image.format() == QImage::Format_Indexed8);
	const int minMajor = 20;
	const int maxMajor = 50;
	const int minMinor = 10;
	const int minThresh = 31;
	const int minVotes = 20;
	const int tag = qrand();
	qDebug() << "minMajor << maxMajor << minMinor << minThresh << minVotes << tag"
			 <<  minMajor << maxMajor << minMinor << minThresh << minVotes << tag;
	const int minMinorSq = minMinor * minMinor;
	const int twoMinMajor = 2 * minMajor;
	const int minMajorSq = minMajor * minMajor;
	const int twoMinMajorSq = twoMinMajor * twoMinMajor;
	const int twoMaxMajorSq = maxMajor * maxMajor * 4;
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
	qDebug() << "has" << pointsCount << "points";
	const int pixelCount = w * h;
	QVector<QAtomicInt> r(pixelCount);
	QAtomicInt *rPtr = r.data();
	int count = 0;
	QAtomicInt progress = 0;
	// (3)
#pragma omp parallel for schedule(guided)
	for (int i = 0; i < pointsCount; i++) {
		QElapsedTimer et;
		et.start();
		const QPoint &p1 = points.at(i);
		const int x1 = p1.x();
		const int y1 = p1.y();
		// (4)
		for (int j = i + 1; j < pointsCount; j++) {
			const QPoint &p2 = points.at(j);
			const int x2 = p2.x();
			const int y2 = p2.y();
			const int dy = y2 - y1;
			const int dx = x2 - x1;
			const int distSq = dx * dx + dy * dy;
			if (Q_LIKELY(distSq < twoMinMajorSq || distSq > twoMaxMajorSq)) {
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
				if (Q_LIKELY(dSq < minMajorSq || dSq > twoMaxMajorSq)) {
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
				if (iMinorSq >= minMinorSq) {
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
				//qDebug() << "boohoo, no ellipse for me... max vote:" << maxVote << x0 << y0 << "a" << votes.keys().at(maxVoteIdx) << major;
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
	rmap.save("vote_map_" + QString::number(tag) + ".png");
	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			if (qGray(rmap.pixel(x, y)) > 127) {
				QVector<int> histogram(256);
				{
					const int size = 15;
					const int y1start = qMax(0, y - size);
					const int y1end = qMin(y + size, h);
					const int x1start = qMax(0, x - size);
					const int x1end = qMin(x + size, w);
					for (int y1 = y1start; y1 < y1end; y1++) {
						for (int x1 = x1start; x1 < x1end; x1++) {
							const int g = qGray(org.pixel(x1, y1));
							histogram[g]++;
						}
					}
				}
				int sum = 0;
				int sum2 = 0;
				for (int i = 0; i < histogram.size(); i++) {
					sum += histogram.at(i);
					if (qBound(lower, i, upper) == i) {
						sum2 += histogram.at(i);
					}
				}
				if (((sum2 * 100) / sum) > 70) {
					qDebug() << "ellipse detected!";
					const int size = minMajor;
					const int y1start = qMax(0, y - size);
					const int y1end = qMin(y + size, h);
					const int x1start = qMax(0, x - size);
					const int x1end = qMin(x + size, w);
					for (int y1 = y1start; y1 < y1end; y1++) {
						for (int x1 = x1start; x1 < x1end; x1++) {
							rmap.setPixel(x1, y1, qRgb(0, 0, 0));
						}
					}
				}
				Ellipse e(QPointF(x, y), 0, 0, 0);
				result.append(e);
			}
		}
	}
	return result;
}
