/****************************************************************************
**
** Jreen
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
**
*****************************************************************************
**
** $JREEN_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $JREEN_END_LICENSE$
**
****************************************************************************/

#ifndef JREEN_ZLIBDATASTREAM_P_H
#define JREEN_ZLIBDATASTREAM_P_H

#include "datastream.h"

namespace Jreen
{
	class ZLibDataStreamPrivate;
	class ZLibDataStream : public DataStream
	{
		Q_OBJECT
		Q_DECLARE_PRIVATE(ZLibDataStream)
	public:
		ZLibDataStream();
		~ZLibDataStream();
		
		qint64 bytesAvailable() const;
		bool open(OpenMode mode);
		void close();
	protected:
		void incomingDataReady();
		qint64 writeData(const char *data, qint64 len);
		qint64 readData(char *data, qint64 maxlen);
	private:
		QScopedPointer<ZLibDataStreamPrivate> d_ptr;
	};
}

#endif // JREEN_ZLIBDATASTREAM_P_H
