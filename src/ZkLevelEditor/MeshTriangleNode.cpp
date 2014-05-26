#include "MeshTriangleNode.h"
#include "MeshLayer.h"

#include <QtCore>
#include <QtGui>
#include <QDebug>

using namespace Zk::LevelEditor;

MeshTriangleNode::MeshTriangleNode(MeshLayer * ml, QGraphicsItem * parent)
	: QGraphicsEllipseItem(parent)
{
	parentLayer = ml;
	
	setFlag(QGraphicsItem::ItemIsSelectable, true);
	setFlag(QGraphicsItem::ItemIsMovable, true);
	setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
	setZValue(1.0);
	
	marked = false;
	refreshLook();
}

MeshTriangleNode::~MeshTriangleNode()
{
	
}

void MeshTriangleNode::addTriangleLink(MeshTriangle * mt)
{
	linkedTriangles << mt;
}

int MeshTriangleNode::getTriangleLinkCount() const
{
	return linkedTriangles.size();
}

void MeshTriangleNode::remTriangleLink(MeshTriangle * mt)
{
	//qDebug() << linkedTriangles;
	qDebug() << "Node ~/~ Triangle";
	linkedTriangles.removeOne(mt);
	
	//qDebug() << mt;
	
	if (linkedTriangles.size() == 0 && linkedEdges.size() == 0)
		emit unlinked(this);
	
	//qDebug() << linkedTriangles.size() << linkedEdges.size();
}

void MeshTriangleNode::addEdgeLink(MeshTriangleEdge * mt)
{
	linkedEdges << mt;
}

int MeshTriangleNode::getEdgeLinkCount() const
{
	return linkedEdges.size();
}

void MeshTriangleNode::remEdgeLink(MeshTriangleEdge * mte)
{
	//qDebug() << "Node ~/~ Edge";
	linkedEdges.removeOne(mte);
	
	if (linkedTriangles.size() == 0 && linkedEdges.size() == 0)
		emit unlinked(this);
	
	//qDebug() << linkedTriangles.size() << linkedEdges.size();
}

const QList<MeshTriangle*> & MeshTriangleNode::getLinkedTriangles() const
{
	return linkedTriangles;
}

const QList<MeshTriangleEdge*> & MeshTriangleNode::getLinkedEdges() const
{
	return linkedEdges;
}

void MeshTriangleNode::setMarked(bool marked)
{
	this->marked = marked;
	refreshLook();
}

bool MeshTriangleNode::isMarked() const
{
	return marked;
}

void MeshTriangleNode::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
	QGraphicsEllipseItem::mousePressEvent(event);
	emit clicked(this, event);
}

void MeshTriangleNode::contextMenuEvent(QGraphicsSceneContextMenuEvent * event)
{
	QMenu menu;
	qDebug() << "DUPA";
	QAction * removeAction = menu.addAction("&Remove");
	menu.exec(event->screenPos());
	
	event->accept();
}

QVariant MeshTriangleNode::itemChange(
	GraphicsItemChange change,
	const QVariant & value
)
{
	if (change == ItemPositionChange)
		emit moved(this, value.toPointF());
	
	return QGraphicsEllipseItem::itemChange(change, value);
}

void MeshTriangleNode::refreshLook()
{
	if (marked)
		setPen(QPen(QBrush(Qt::blue), 3.0));
	else
		setPen(QPen(QBrush(Qt::white), 2.0));
	
	setBrush(QBrush(QColor(0, 0, 0)));
	setRect(QRect(-8, -8, 16, 16));
}
