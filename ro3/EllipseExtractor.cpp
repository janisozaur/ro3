#include "EllipseExtractor.h"

#include <qmath.h>
#include <QElapsedTimer>

#include <QDebug>

#define ABS(x) ((x) < 0 ? -(x) : (x))

EllipseExtractor::EllipseExtractor()
{
}

QList<Ellipse> EllipseExtractor::extract(const QImage &image) const
{
	Q_ASSERT(image.format() == QImage::Format_Indexed8);
	const int minDist = 35;
	const int minThresh = 25;
	const int minVotes = 30;
	QList<Ellipse> result;
	const int w = image.width();
	const int h = image.height();
	const int pixelCount = w * h;
	int count = 0;
	// (3)
	for (int y1 = 0; y1 < h; y1++) {
		const uchar *d1 = image.constScanLine(y1);
		for (int x1 = 0; x1 < w; x1++) {
			if (d1[x1] < minThresh) {
				continue;
			}
			QElapsedTimer et;
			et.start();
			// (4)
			for (int y2 = 0; y2 < h; y2++) {
				const uchar *d2 = image.constScanLine(y2);
				for (int x2 = 0; x2 < w; x2++) {
					if (d2[x2] < minThresh) {
						continue;
					}
					const int dy = y2 - y1;
					const int dx = x2 - x1;
					const qreal dist = qSqrt(dx * dx + dy * dy);
					if (dist < minDist) {
						continue;
					}
					// (5)
					const qreal x0 = qreal(x1 + x2) / 2;
					const qreal y0 = qreal(y1 + y2) / 2;
					const qreal major = dist / 2;
					const qreal majorSq = major * major;
					const qreal orientation = atan(qreal(dy) / qreal(dx));
					QHash<long long, int> votes;
					// (6)
					for (int y = 0; y < h; y++) {
						const uchar *d = image.constScanLine(y);
						for (int x = 0; x < w; x++) {
							if (d[x] < minThresh) {
								continue;
							}
							const int dyi = y0 - y;
							const int dxi = x0 - x;
							const qreal dSq = dxi * dxi + dyi * dyi;
							const qreal d = qSqrt(dSq);
							// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
							// NOTE: this here might also include "d > major"
							// condition, for we don't want any point that is
							// further than the tip of major axis
							// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
							if (d < minDist) {
								continue;
							}
							// (7)
							const int fy = y2 - y;
							const int fx = x2 - x;
							const qreal fSq = fy * fy + fx * fx;
							const qreal cosTau = (majorSq + dSq - fSq) / (2 * major * d);
							const qreal cosTauSq = cosTau * cosTau;
							const qreal sinTauSq = 1 - cosTauSq;
							const qreal nom = majorSq * dSq * sinTauSq;
							const qreal den = majorSq - dSq * cosTauSq;
							// (8)
							if (den != 0) {
								const qreal minorSq = nom / den;
								long long iMinorSq = minorSq;
								if (iMinorSq < 0) {
									iMinorSq = -iMinorSq;
								}
								//if (qBound(0, minorSq, votes.size() - 1) == minorSq) {
									votes[iMinorSq]++;
								//}
							}
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
						//qDebug() << "boohoo, no ellipse for me... max vote:" << maxVote;
					}
				}
			}
			const int msecs = et.elapsed();
			qDebug() << "msecs:" << msecs << ", " << qreal(y1 * w + x1) / qreal(pixelCount) * 100 << "%";
		}
	}
	return result;
}
