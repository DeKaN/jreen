/****************************************************************************
 *
 *  This file is part of qutIM
 *
 *  Copyright (c) 2011 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This file is part of free software; you can redistribute it and/or    *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************
 ****************************************************************************/

#include "pubsubevent.h"
#include "pubsubmanager_p.h"

namespace Jreen
{
namespace PubSub
{
class EventPrivate
{
public:
	QList<Payload::Ptr> items;
	QString node;
};

Event::Event(const QString &node) : d_ptr(new EventPrivate)
{
	Q_D(Event);
	d->node = node;
}

Event::Event(const Payload::Ptr &item) : d_ptr(new EventPrivate)
{
	Q_D(Event);
	d->items << item;
}

Event::Event(const QList<Payload::Ptr> &items) : d_ptr(new EventPrivate)
{
	Q_D(Event);
	d->items = items;
}

Event::~Event()
{
}

void Event::setNode(const QString &node)
{
	d_func()->node = node;
}

QString Event::node() const
{
	return d_func()->node;
}

void Event::addItem(Payload *item)
{
	d_func()->items << Payload::Ptr(item);
}

void Event::addItem(const Payload::Ptr &item)
{
	d_func()->items << item;
}

QList<Payload::Ptr> Event::items() const
{
	return d_func()->items;
}
}
}
