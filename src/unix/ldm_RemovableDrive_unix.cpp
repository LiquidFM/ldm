/**
 * This file is part of ldm.
 *
 * Copyright (C) 2011-2016 Dmitriy Vilkov, <dav.daemon@gmail.com>
 *
 * ldm is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ldm is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ldm. If not, see <http://www.gnu.org/licenses/>.
 */

#include "../ldm_RemovableDrive.h"
#include "../ldm_Partition.h"
#include "ldm_udisks.h"


namespace LDM {

bool RemovableDrive::detach(QString &error)
{
	QDBusInterface interface(QString::fromLatin1(UD_DBUS_SERVICE), id(), QString::fromLatin1(UD_DBUS_INTERFACE_DISKS_DEVICE), QDBusConnection::systemBus());

	if (interface.isValid())
	{
		const Container &p = partitions();

		for (Container::const_iterator i = p.constBegin(), end = p.constEnd(); i != end; ++i)
			if (!(*i)->mountPaths().isEmpty())
				if (!(*i)->unmount(error))
					return false;

		QDBusMessage message = QDBusMessage::createMethodCall(QString::fromLatin1(UD_DBUS_SERVICE),
															  id(),
															  QString::fromLatin1(UD_DBUS_INTERFACE_DISKS_DEVICE),
															  QString::fromLatin1("DriveDetach"));

		message.setArguments(QList<QVariant>() << QStringList());

		QDBusReply<void> reply = QDBusConnection::systemBus().call(message);

		if (reply.isValid())
			return true;
		else
			error = reply.error().message();
	}
	else
		error = interface.lastError().message();

	return false;
}

bool RemovableDrive::eject(QString &error)
{
	QDBusInterface interface(QString::fromLatin1(UD_DBUS_SERVICE), id(), QString::fromLatin1(UD_DBUS_INTERFACE_DISKS_DEVICE), QDBusConnection::systemBus());

	if (interface.isValid())
	{
		const Container &p = partitions();

		for (Container::const_iterator i = p.constBegin(), end = p.constEnd(); i != end; ++i)
			if (!(*i)->mountPaths().isEmpty())
				if (!(*i)->unmount(error))
					return false;

		QDBusMessage message = QDBusMessage::createMethodCall(QString::fromLatin1(UD_DBUS_SERVICE),
															  id(),
															  QString::fromLatin1(UD_DBUS_INTERFACE_DISKS_DEVICE),
															  QString::fromLatin1("DriveEject"));

		message.setArguments(QList<QVariant>() << QStringList());

		QDBusReply<void> reply = QDBusConnection::systemBus().call(message);

		if (reply.isValid())
			return true;
		else
			error = reply.error().message();
	}
	else
		error = interface.lastError().message();

	return false;
}

}
