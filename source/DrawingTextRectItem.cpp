/* DrawingTextRectItem.cpp
 *
 * Copyright (C) 2013-2016 Jason Allen
 *
 * This file is part of the jade library.
 *
 * jade is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * jade is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with jade.  If not, see <http://www.gnu.org/licenses/>
 */

#include "DrawingTextRectItem.h"
#include "DrawingWidget.h"
#include "DrawingItemPoint.h"
#include "DrawingItemStyle.h"
#include "DrawingUndo.h"

DrawingTextRectItem::DrawingTextRectItem() : DrawingItem()
{
	mCornerRadiusX = 0;
	mCornerRadiusY = 0;
	mCaption = "Label";

	DrawingItemPoint::Flags flags = (DrawingItemPoint::Control | DrawingItemPoint::Connection);
	for(int i = 0; i < 8; i++) addPoint(new DrawingItemPoint(QPointF(0, 0), flags));
	setRect(QRectF(-400, -200, 800, 400));

	DrawingItemStyle* style = DrawingItem::style();
	style->setValue(DrawingItemStyle::PenStyle,
		style->valueLookup(DrawingItemStyle::PenStyle, QVariant((uint)Qt::SolidLine)));
	style->setValue(DrawingItemStyle::PenColor,
		style->valueLookup(DrawingItemStyle::PenColor, QVariant(QColor(0, 0, 0))));
	style->setValue(DrawingItemStyle::PenOpacity,
		style->valueLookup(DrawingItemStyle::PenOpacity, QVariant(1.0)));
	style->setValue(DrawingItemStyle::PenWidth,
		style->valueLookup(DrawingItemStyle::PenWidth, QVariant(12.0)));
	style->setValue(DrawingItemStyle::PenCapStyle,
		style->valueLookup(DrawingItemStyle::PenCapStyle, QVariant((uint)Qt::RoundCap)));
	style->setValue(DrawingItemStyle::PenJoinStyle,
		style->valueLookup(DrawingItemStyle::PenJoinStyle, QVariant((uint)Qt::RoundJoin)));

	style->setValue(DrawingItemStyle::BrushStyle,
		style->valueLookup(DrawingItemStyle::BrushStyle, QVariant((uint)Qt::SolidPattern)));
	style->setValue(DrawingItemStyle::BrushColor,
		style->valueLookup(DrawingItemStyle::BrushColor, QVariant(QColor(255, 255, 255))));
	style->setValue(DrawingItemStyle::BrushOpacity,
		style->valueLookup(DrawingItemStyle::BrushOpacity, QVariant(1.0)));

	style->setValue(DrawingItemStyle::TextColor,
		style->valueLookup(DrawingItemStyle::TextColor, QVariant(QColor(0, 0, 0))));
	style->setValue(DrawingItemStyle::TextOpacity,
		style->valueLookup(DrawingItemStyle::TextOpacity, QVariant(1.0)));

	style->setValue(DrawingItemStyle::FontName,
		style->valueLookup(DrawingItemStyle::FontName, QVariant("Arial")));
	style->setValue(DrawingItemStyle::FontSize,
		style->valueLookup(DrawingItemStyle::FontSize, QVariant(100.0)));
	style->setValue(DrawingItemStyle::FontBold,
		style->valueLookup(DrawingItemStyle::FontBold, QVariant(false)));
	style->setValue(DrawingItemStyle::FontItalic,
		style->valueLookup(DrawingItemStyle::FontItalic, QVariant(false)));
	style->setValue(DrawingItemStyle::FontUnderline,
		style->valueLookup(DrawingItemStyle::FontUnderline, QVariant(false)));
	style->setValue(DrawingItemStyle::FontOverline,
		style->valueLookup(DrawingItemStyle::FontOverline, QVariant(false)));
	style->setValue(DrawingItemStyle::FontStrikeThrough,
		style->valueLookup(DrawingItemStyle::FontStrikeThrough, QVariant(false)));
}

DrawingTextRectItem::DrawingTextRectItem(const DrawingTextRectItem& item) : DrawingItem(item)
{
	mCornerRadiusX = item.mCornerRadiusX;
	mCornerRadiusY = item.mCornerRadiusY;
	mCaption = item.mCaption;
}

DrawingTextRectItem::~DrawingTextRectItem() { }

//==================================================================================================

DrawingItem* DrawingTextRectItem::copy() const
{
	return new DrawingTextRectItem(*this);
}

//==================================================================================================

void DrawingTextRectItem::setRect(const QRectF& rect)
{
	QList<DrawingItemPoint*> points = DrawingTextRectItem::points();
	points[0]->setPos(rect.left(), rect.top());
	points[1]->setPos(rect.center().x(), rect.top());
	points[2]->setPos(rect.right(), rect.top());
	points[3]->setPos(rect.right(), rect.center().y());
	points[4]->setPos(rect.right(), rect.bottom());
	points[5]->setPos(rect.center().x(), rect.bottom());
	points[6]->setPos(rect.left(), rect.bottom());
	points[7]->setPos(rect.left(), rect.center().y());
}

void DrawingTextRectItem::setRect(qreal left, qreal top, qreal width, qreal height)
{
	setRect(QRectF(left, top, width, height));
}

QRectF DrawingTextRectItem::rect() const
{
	QList<DrawingItemPoint*> points = DrawingTextRectItem::points();
	return (points.size() >= 8) ? QRectF(points[0]->pos(), points[4]->pos()) : QRectF();
}

//==================================================================================================

void DrawingTextRectItem::setCornerRadii(qreal radiusX, qreal radiusY)
{
	mCornerRadiusX = radiusX;
	mCornerRadiusY = radiusY;
}

qreal DrawingTextRectItem::cornerRadiusX() const
{
	return mCornerRadiusX;
}

qreal DrawingTextRectItem::cornerRadiusY() const
{
	return mCornerRadiusY;
}

//==================================================================================================

void DrawingTextRectItem::setCaption(const QString& caption)
{
	mCaption = caption;
}

QString DrawingTextRectItem::caption() const
{
	return mCaption;
}

//==================================================================================================

QRectF DrawingTextRectItem::boundingRect() const
{
	QRectF rect;

	if (isValid())
	{
		qreal penWidth = style()->valueLookup(DrawingItemStyle::PenWidth).toReal();

		rect = DrawingTextRectItem::rect().normalized();
		rect.adjust(-penWidth/2, -penWidth/2, penWidth/2, penWidth/2);

		rect = rect.united(calculateTextRect(mCaption, style()->font()));
	}

	return rect;
}

QPainterPath DrawingTextRectItem::shape() const
{
	QPainterPath shape;

	if (isValid())
	{
		QPainterPath drawPath;

		DrawingItemStyle* style = DrawingItem::style();
		QPen pen = style->pen();
		QBrush brush = style->brush();
		QFont font = style->font();

		// Add rect
		drawPath.addRoundedRect(rect().normalized(), mCornerRadiusX, mCornerRadiusY);

		// Determine outline path
		pen.setWidthF(qMax(pen.widthF(), minimumPenWidth()));
		shape = strokePath(drawPath, pen);

		if (brush.color().alpha() > 0) shape.addPath(drawPath);

		// Add text
		shape.addRect(calculateTextRect(mCaption, font));
	}

	return shape;
}

bool DrawingTextRectItem::isValid() const
{
	QList<DrawingItemPoint*> points = DrawingTextRectItem::points();
	return (points.size() >= 8 && points[0]->pos() != points[4]->pos() && !mCaption.isEmpty());
}

//==================================================================================================

void DrawingTextRectItem::paint(QPainter* painter)
{
	if (isValid())
	{
		QBrush sceneBrush = painter->brush();
		QPen scenePen = painter->pen();
		QFont sceneFont = painter->font();

		DrawingItemStyle* style = DrawingItem::style();
		QPen pen = style->pen();
		QBrush brush = style->brush();
		QFont font = style->font();
		QBrush textBrush = style->textBrush();

		// Draw rect
		painter->setBrush(brush);
		painter->setPen(pen);
		painter->drawRoundedRect(rect(), mCornerRadiusX, mCornerRadiusY);

		// Draw text
		QPen textPen(textBrush, 1, Qt::SolidLine);
		painter->setBrush(Qt::transparent);
		painter->setPen(textPen);
		painter->setFont(font);
		painter->drawText(calculateTextRect(mCaption, font), Qt::AlignCenter, mCaption);

		painter->setBrush(sceneBrush);
		painter->setPen(scenePen);
		painter->setFont(sceneFont);
	}

	// Draw shape (debug)
	//painter->setBrush(QColor(255, 0, 255, 128));
	//painter->setPen(QPen(painter->brush(), 1));
	//painter->drawPath(shape());
	//painter->drawRect(boundingRect());
}

//==================================================================================================

void DrawingTextRectItem::resizeItem(DrawingItemPoint* itemPoint, const QPointF& scenePos)
{
	DrawingItem::resizeItem(itemPoint, scenePos);

	QList<DrawingItemPoint*> points = DrawingTextRectItem::points();
	if (points.size() >= 8)
	{
		QRectF rect = DrawingTextRectItem::rect();
		int pointIndex = points.indexOf(itemPoint);

		if (0 <= pointIndex && pointIndex < 8)
		{
			switch (pointIndex)
			{
			case 0:	rect.setTopLeft(itemPoint->pos()); break;
			case 1:	rect.setTop(itemPoint->y()); break;
			case 2:	rect.setTopRight(itemPoint->pos()); break;
			case 3:	rect.setRight(itemPoint->x()); break;
			case 4:	rect.setBottomRight(itemPoint->pos()); break;
			case 5:	rect.setBottom(itemPoint->y()); break;
			case 6:	rect.setBottomLeft(itemPoint->pos()); break;
			case 7:	rect.setLeft(itemPoint->x()); break;
			default: break;
			}

			points[0]->setPos(rect.left(), rect.top());
			points[1]->setPos(rect.center().x(), rect.top());
			points[2]->setPos(rect.right(), rect.top());
			points[3]->setPos(rect.right(), rect.center().y());
			points[4]->setPos(rect.right(), rect.bottom());
			points[5]->setPos(rect.center().x(), rect.bottom());
			points[6]->setPos(rect.left(), rect.bottom());
			points[7]->setPos(rect.left(), rect.center().y());
		}
	}

	// Adjust position of item and item points so that point(0)->pos() == QPointF(0, 0)
	QPointF deltaPos = -points.first()->pos();
	QPointF pointScenePos = mapToScene(points.first()->pos());

	for(auto pointIter = points.begin(); pointIter != points.end(); pointIter++)
		(*pointIter)->setPos((*pointIter)->pos() + deltaPos);

	setPos(pointScenePos);
}

//==================================================================================================

QRectF DrawingTextRectItem::calculateTextRect(const QString& caption, const QFont& font) const
{
	qreal textWidth = 0, textHeight = 0;
	QRectF pointsRect = DrawingTextRectItem::rect();

	QFontMetricsF fontMetrics(font);
	QStringList lines = caption.split("\n");

	for(auto lineIter = lines.begin(); lineIter != lines.end(); lineIter++)
	{
		textWidth = qMax(textWidth, fontMetrics.width(*lineIter));
		textHeight += fontMetrics.lineSpacing();
	}

	textHeight -= fontMetrics.leading();

	return QRectF(-textWidth / 2, -textHeight / 2, textWidth, textHeight).translated(pointsRect.center());
}

