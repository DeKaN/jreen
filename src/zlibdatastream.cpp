/****************************************************************************
 *
 *  This file is part of qutIM
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

#include "zlibdatastream.h"
#include <zlib.h>
#include <QBasicTimer>
#include <QTimerEvent>

namespace jreen
{
	class ZLibDataStreamPrivate
	{
	public:
		z_stream zinflate;
		z_stream zdeflate;
		QByteArray buffer;
		int offset;
		int len;
		QByteArray outBuffer;
		QBasicTimer timer;
		void ensureSize(int size)
		{
			if (buffer.size() - offset - len < size)
				buffer.resize(offset + len + size);
		}
	};
	
	ZLibDataStream::ZLibDataStream() : d_ptr(new ZLibDataStreamPrivate)
	{
		Q_D(ZLibDataStream);
		memset(&d->zinflate, 0, sizeof(z_stream));
		memset(&d->zdeflate, 0, sizeof(z_stream));
		d->offset = 0;
		d->len = 0;
	}
	
	ZLibDataStream::~ZLibDataStream()
	{
	}
	
	qint64 ZLibDataStream::size() const
	{
		return d_func()->len + QIODevice::size();
	}
	
	bool ZLibDataStream::open(OpenMode mode)
	{
		Q_D(ZLibDataStream);
		memset(&d->zinflate, 0, sizeof(z_stream));
		memset(&d->zdeflate, 0, sizeof(z_stream));
		int res = inflateInit(&d->zinflate);
		if (res != Z_OK)
			return false;
		res = deflateInit(&d->zdeflate, Z_BEST_COMPRESSION);
		if (res != Z_OK) {
			inflateEnd(&d->zinflate);
			return false;
		}
		QIODevice::open(mode);
		return true;
	}

	void ZLibDataStream::close()
	{
		Q_D(ZLibDataStream);
		if (!isOpen())
			return;
		inflateEnd(&d->zinflate);
		deflateEnd(&d->zdeflate);
	}
	
	void ZLibDataStream::timerEvent(QTimerEvent *event)
	{
		if (event->timerId() == d_func()->timer.timerId()) {
			flush();
			d_func()->timer.stop();
		}
		return QIODevice::timerEvent(event);
	}

	void ZLibDataStream::incomingDataReady()
	{
		Q_D(ZLibDataStream);
		QByteArray data = device()->readAll();
		const char chunkSize = 100;
		d->zinflate.next_in = reinterpret_cast<Bytef*>(data.data());
		d->zinflate.avail_in = data.size();
		do {
			d->ensureSize(chunkSize);
			d->zinflate.avail_out = chunkSize;
			d->zinflate.next_out = reinterpret_cast<Bytef*>(d->buffer.data() + d->offset + d->len);
			inflate(&d->zinflate, Z_SYNC_FLUSH);
			d->len += chunkSize - d->zinflate.avail_out;
		} while (d->zinflate.avail_out == 0);
		emit readyRead();
	}

	qint64 ZLibDataStream::writeData(const char *data, qint64 len)
	{
		Q_D(ZLibDataStream);
		if (len <= 0)
			return 0;
		d->outBuffer.append(data, len);
		if (!d->timer.isActive())
			d->timer.start(0, this);
		return len;
	}
	
	void ZLibDataStream::flush()
	{
		Q_D(ZLibDataStream);
		d->zdeflate.avail_in = d->outBuffer.size();
		d->zdeflate.next_in = reinterpret_cast<Bytef*>(const_cast<char *>(d->outBuffer.data()));
		const int chunkSize = d->zdeflate.avail_in * 1.01 + 13;
		QByteArray buffer(chunkSize, Qt::Uninitialized);
		do {
			d->zdeflate.avail_out = buffer.size();
			d->zdeflate.next_out = reinterpret_cast<Bytef*>(buffer.data());
			deflate(&d->zdeflate, Z_SYNC_FLUSH);
			device()->write(buffer.data(), chunkSize - d->zdeflate.avail_out);
		} while (d->zdeflate.avail_out == 0);
		d->outBuffer.clear();
	}

	qint64 ZLibDataStream::readData(char *data, qint64 maxlen)
	{
		Q_D(ZLibDataStream);
		int len = qMin<int>(maxlen, d->len);
		qMemCopy(data, d->buffer.data() + d->offset, len);
		if (maxlen < d->len) {
			d->len -= maxlen;
			d->offset += maxlen;
		} else {
			d->offset = 0;
			d->len = 0;
		}
		return len;
	}
}
