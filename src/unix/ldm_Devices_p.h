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

#ifndef LDM_DEVICES_P_H_
#define LDM_DEVICES_P_H_

#include "ldm_udisks.h"
#include "ldm_Partition_unix.h"

#include "../ldm_Visitor.h"
#include "../ldm_Devices.h"
#include "../ldm_HardDrive.h"
#include "../ldm_FlashDrive.h"
#include "../ldm_FloppyDrive.h"
#include "../ldm_OpticalDrive.h"
#include "../ldm_RemovableDrive.h"

#include <efc/ScopedPointer>

#include <lvfs/Module>
#include <lvfs/IEntry>

#include <QtCore/QTextCodec>


using namespace LDM;

class Devices::Private : public QObject, public Visitor
{
    Q_OBJECT

public:
    class Remover : public Visitor
    {
    public:
        typedef void (Private::*Method1)(Partition &);
        typedef void (Private::*Method2)(Device &);

    public:
        Remover(Private *parent, Method1 m1, Method2 m2) :
            parent(parent),
            m1(m1),
            m2(m2)
        {}

        virtual void visit(Drive &item)  { (parent->*m2)(item); }
        virtual void visit(HardDrive &item)  { (parent->*m2)(item); }
        virtual void visit(FlashDrive &item)  { (parent->*m2)(item); }
        virtual void visit(RemovableDrive &item)  { (parent->*m2)(item); }
        virtual void visit(OpticalDrive &item)  { (parent->*m2)(item); }
        virtual void visit(FloppyDrive &item) { (parent->*m2)(item); }
        virtual void visit(Partition &item) { (parent->*m1)(item); }

    private:
        Private *parent;
        Method1 m1;
        Method2 m2;
    };

public:
    Private(Devices *parent) :
        QObject(NULL),
        parent(parent),
        manager(QString::fromLatin1(UD_DBUS_SERVICE),
                QString::fromLatin1(UD_DBUS_PATH),
                QString::fromLatin1(UD_DBUS_INTERFACE_DISKS),
                QDBusConnection::systemBus())
    {
//      qDBusRegisterMetaType<QList<QDBusObjectPath> >();
//      qDBusRegisterMetaType<QVariantMap>();

        bool serviceFound = manager.isValid();

        if (!serviceFound)
        {
            // find out whether it will be activated automatically
            QDBusMessage message = QDBusMessage::createMethodCall(QString::fromLatin1("org.freedesktop.DBus"),
                                                                  QString::fromLatin1("/org/freedesktop/DBus"),
                                                                  QString::fromLatin1("org.freedesktop.DBus"),
                                                                  QString::fromLatin1("ListActivatableNames"));

            QDBusReply<QStringList> reply = QDBusConnection::systemBus().call(message);

            if (reply.isValid() && reply.value().contains(QString::fromLatin1(UD_DBUS_SERVICE)))
            {
                QDBusConnection::systemBus().interface()->startService(QString::fromLatin1(UD_DBUS_SERVICE));
                serviceFound = true;
            }
        }

        if (serviceFound)
        {
            connect(&manager, SIGNAL(DeviceAdded(QDBusObjectPath)),
                    this, SLOT(slotDeviceAdded(QDBusObjectPath)));
            connect(&manager, SIGNAL(DeviceRemoved(QDBusObjectPath)),
                    this, SLOT(slotDeviceRemoved(QDBusObjectPath)));
            connect(&manager, SIGNAL(DeviceChanged(QDBusObjectPath)),
                    this, SLOT(slotDeviceChanged(QDBusObjectPath)));
        }
    }

    virtual ~Private()
    {
        qDeleteAll(m_topLevelDevices);
    }

public:
    static QString driveLabel(const QDBusInterface &interface)
    {
        QString res = interface.property("DevicePresentationName").toString();

        if (res.isEmpty())
            res = interface.property("DriveModel").toString();

        return res;
    }

    static QString partitionLabel(const QDBusInterface &interface, const QStringList &mountPaths)
    {
        QString res = interface.property("DevicePresentationName").toString();

        if (res.isEmpty())
        {
            res = interface.property("PartitionLabel").toString();

            if (res.isEmpty())
            {
                res = interface.property("IdLabel").toString();

                if (res.isEmpty())
                    if (interface.property("DeviceIsMounted").toBool())
                        res = mountPaths.at(0);
                    else
                    {
                        res = interface.property("DeviceFilePresentation").toString();

                        if (res.isEmpty())
                            res = interface.property("DeviceFile").toString();
                    }
            }
        }

        return res;
    }

    static QIcon mediaTypeIconByConnectionInterface(const QDBusInterface &interface)
    {
        static const QString usbInterface = QString::fromLatin1("usb");

        using namespace LVFS;
        Interface::Holder res;
        QIcon icon;

        if (interface.property("DriveConnectionInterface").toString() == usbInterface)
            res = Module::desktop().theme().icon(Desktop::Theme::Devices::RemovableMediaUsb);
        else
            res = Module::desktop().theme().icon(Desktop::Theme::Devices::RemovableMedia);

        if (res.isValid())
        {
            QTextCodec *codec = QTextCodec::codecForLocale();
            icon.addFile(codec->toUnicode(res->as<IEntry>()->location()), QSize(Desktop::SmallIcon, Desktop::SmallIcon));
        }

        return icon;
    }

    Drive *drive(QIcon &icon, const QString &path, const QDBusInterface &interface)
    {
        QString string = interface.property("DevicePresentationIconName").toString();
        Drive::MediaTypeSet typeSet = Drive::stringListToMediaTypeSet(interface.property("DriveMediaCompatibility").toStringList());

        if (!string.isEmpty())
        {
            using namespace LVFS;
            Interface::Holder res = Module::desktop().theme().icon(string.toUtf8());

            if (res.isValid())
            {
                QTextCodec *codec = QTextCodec::codecForLocale();
                icon.addFile(codec->toUnicode(res->as<IEntry>()->location()), QSize(Desktop::SmallIcon, Desktop::SmallIcon));
            }
        }

        if (!Drive::MediaTypeSet(typeSet).intersect(FlashDrive::mediaTypeSet()).isEmpty())
        {
            Drive::MediaType type = Drive::stringToMediaType(interface.property("DriveMedia").toString());

            if (icon.isNull() && (icon = Drive::mediaTypeIcon(type)).isNull())
                icon = mediaTypeIconByConnectionInterface(interface);

            return new FlashDrive(path,
                                  icon,
                                  driveLabel(interface),
                                  interface.property("DevicePresentationHide").toBool(),
                                  NULL,
                                  interface.property("DeviceSize").toULongLong(),
                                  typeSet,
                                  type,
                                  interface.property("DriveCanDetach").toBool());
        }
        else if (!Drive::MediaTypeSet(typeSet).intersect(OpticalDrive::mediaTypeSet()).isEmpty())
        {
            EFC::ScopedPointer<OpticalDrive> drive;

            if (icon.isNull())
            {
                using namespace LVFS;
                Interface::Holder res = Module::desktop().theme().icon(Desktop::Theme::Devices::Optical);

                if (res.isValid())
                {
                    QTextCodec *codec = QTextCodec::codecForLocale();
                    icon.addFile(codec->toUnicode(res->as<IEntry>()->location()), QSize(Desktop::SmallIcon, Desktop::SmallIcon));
                }
            }

            drive.reset(new OpticalDrive(path,
                                         icon,
                                         driveLabel(interface),
                                         interface.property("DevicePresentationHide").toBool(),
                                         NULL,
                                         interface.property("DeviceSize").toULongLong(),
                                         typeSet,
                                         Drive::stringToMediaType(interface.property("DriveMedia").toString()),
                                         interface.property("DriveCanDetach").toBool(),
                                         interface.property("DriveIsMediaEjectable").toBool()));

            if (interface.property("DeviceIsMediaAvailable").toBool())
                opticalDrivePartition(drive.get(), path, interface);

            return drive.release();
        }
        else if (!Drive::MediaTypeSet(typeSet).intersect(FloppyDrive::mediaTypeSet()).isEmpty())
        {
            Drive::MediaType type = Drive::stringToMediaType(interface.property("DriveMedia").toString());

            if (icon.isNull() && (icon = Drive::mediaTypeIcon(type)).isNull())
                icon = mediaTypeIconByConnectionInterface(interface);

            return new FloppyDrive(path,
                                   icon,
                                   driveLabel(interface),
                                   interface.property("DevicePresentationHide").toBool(),
                                   NULL,
                                   interface.property("DeviceSize").toULongLong(),
                                   typeSet,
                                   type,
                                   interface.property("DriveCanDetach").toBool());
        }

        return NULL;
    }

    Drive *hardDrive(const QString &path, const QDBusInterface &interface)
    {
        QIcon icon;

        if (Drive *res = drive(icon, path, interface))
            return res;
        else
        {
            if (icon.isNull())
            {
                using namespace LVFS;
                Interface::Holder res = Module::desktop().theme().icon(Desktop::Theme::Devices::HardDisk);

                if (res.isValid())
                {
                    QTextCodec *codec = QTextCodec::codecForLocale();
                    icon.addFile(codec->toUnicode(res->as<IEntry>()->location()), QSize(Desktop::SmallIcon, Desktop::SmallIcon));
                }
            }

            return new HardDrive(path,
                                 icon,
                                 driveLabel(interface),
                                 interface.property("DevicePresentationHide").toBool(),
                                 NULL,
                                 interface.property("DeviceSize").toULongLong());
        }
    }

    Drive *removableDrive(const QString &path, const QDBusInterface &interface)
    {
        QIcon icon;

        if (Drive *res = drive(icon, path, interface))
            return res;
        else
        {
            if (icon.isNull())
                icon = mediaTypeIconByConnectionInterface(interface);

            return new RemovableDrive(path,
                                      icon,
                                      driveLabel(interface),
                                      interface.property("DevicePresentationHide").toBool(),
                                      NULL,
                                      interface.property("DeviceSize").toULongLong(),
                                      Drive::MediaTypeSet(),
                                      Drive::NoMedia,
                                      interface.property("DriveCanDetach").toBool());
        }
    }

public:
    Device *processDevice(const QString &path, const QDBusInterface &interface)
    {
        if (!m_topLevelDevices.contains(path))
            if (interface.property("DeviceIsPartition").toBool())
                partition(path, interface);
            else if (interface.property("DeviceIsRemovable").toBool())
                return removableDrive(path, interface);
            else if (interface.property("DeviceIsDrive").toBool())
                return hardDrive(path, interface);

        return NULL;
    }

    void opticalDrivePartition(OpticalDrive *parent, const QString &path, const QDBusInterface &interface)
    {
        QIcon icon;
        QString string;
        EFC::ScopedPointer<PartitionUnix> partition;

        QStringList mountPaths(interface.property("DeviceMountPaths").toStringList());
        string = interface.property("DevicePresentationIconName").toString();

        if (!string.isEmpty())
        {
            using namespace LVFS;
            Interface::Holder res = Module::desktop().theme().icon(string.toUtf8());

            if (res.isValid())
            {
                QTextCodec *codec = QTextCodec::codecForLocale();
                icon.addFile(codec->toUnicode(res->as<IEntry>()->location()), QSize(Desktop::SmallIcon, Desktop::SmallIcon));
            }
        }

        if (icon.isNull())
            icon = Drive::mediaTypeIcon(parent->media());

        partition.reset(new PartitionUnix(path,
                                          icon,
                                          partitionLabel(interface, mountPaths),
                                          interface.property("DevicePresentationHide").toBool(),
                                          static_cast< Drive *>(parent),
                                          interface.property("PartitionNumber").toInt(),
                                          interface.property("PartitionSize").toULongLong(),
                                          mountPaths));

        parent->addPartition(partition.get());
        this->parent->slotDeviceAdded(partition.release());
    }

    void partition(const QString &path, const QDBusInterface &interface)
    {
        QIcon icon;
        QString string;
        EFC::ScopedPointer<PartitionUnix> partition;
        Device *parent = m_topLevelDevices.value(string = interface.property("PartitionSlave").value<QDBusObjectPath>().path());

        if (parent == NULL)
        {
            QDBusInterface interface(QString::fromLatin1(UD_DBUS_SERVICE), string, QString::fromLatin1(UD_DBUS_INTERFACE_DISKS_DEVICE), QDBusConnection::systemBus());

            if (interface.isValid() && (parent = processDevice(string, interface)))
                this->parent->slotDeviceAdded(m_allDevices[string] = m_topLevelDevices[string] = parent);
        }

        if (parent)
        {
            QStringList mountPaths(interface.property("DeviceMountPaths").toStringList());
            string = interface.property("DevicePresentationIconName").toString();

            if (!string.isEmpty())
            {
                using namespace LVFS;
                Interface::Holder res = Module::desktop().theme().icon(string.toUtf8());

                if (res.isValid())
                {
                    QTextCodec *codec = QTextCodec::codecForLocale();
                    icon.addFile(codec->toUnicode(res->as<IEntry>()->location()), QSize(Desktop::SmallIcon, Desktop::SmallIcon));
                }
            }

            if (icon.isNull())
                icon = parent->icon();

            partition.reset(new PartitionUnix(path,
                                              icon,
                                              partitionLabel(interface, mountPaths),
                                              interface.property("DevicePresentationHide").toBool(),
                                              static_cast< Drive *>(parent),
                                              interface.property("PartitionNumber").toInt(),
                                              interface.property("PartitionSize").toULongLong(),
                                              mountPaths));

            m_allDevices[path] = partition.get();
            parent->as< Drive >()->addPartition(partition.get());
            this->parent->slotDeviceAdded(partition.release());
        }
    }

    void refresh()
    {
        typedef QList<QDBusObjectPath> List;
        typedef QDBusReply<List>       Reply;

        QString string;
        EFC::ScopedPointer<Device> device;
        Reply reply = manager.call(QString::fromLatin1("EnumerateDevices"));

        if (reply.isValid())
        {
            List list(reply.value());

            for (List::size_type i = 0, size = list.size(); i < size; ++i)
            {
                QDBusInterface interface(QString::fromLatin1(UD_DBUS_SERVICE), string = list.at(i).path(), QString::fromLatin1(UD_DBUS_INTERFACE_DISKS_DEVICE), QDBusConnection::systemBus());

                if (interface.isValid())
                {
                    device.reset(processDevice(string, interface));

                    if (device)
                    {
                        m_allDevices[string] = m_topLevelDevices[string] = device.get();
                        parent->slotDeviceAdded(device.release());
                    }
                }
            }
        }
    }

public Q_SLOTS:
    void slotDeviceAdded(const QDBusObjectPath &path)
    {
        EFC::ScopedPointer<Device> device;
        QDBusInterface interface(QString::fromLatin1(UD_DBUS_SERVICE), path.path(), QString::fromLatin1(UD_DBUS_INTERFACE_DISKS_DEVICE), QDBusConnection::systemBus());

        if (interface.isValid())
        {
            device.reset(processDevice(path.path(), interface));

            if (device)
            {
                m_allDevices[path.path()] = m_topLevelDevices[path.path()] = device.get();
                parent->slotDeviceAdded(device.release());
            }
        }
    }

    void slotDeviceRemoved(const QDBusObjectPath &path)
    {
        if (Device *device = m_allDevices.take(path.path()))
        {
            Remover remover(this, &Devices::Private::slotDeviceRemoved, &Devices::Private::slotDeviceRemoved);
            device->accept(remover);
        }
    }

    void slotDeviceRemoved(Partition &item)
    {
        EFC::ScopedPointer<Partition> partition(item.parent()->as<Drive>()->takePartition(item.id()));
        parent->slotDeviceRemoved(partition.get());
    }

    void slotDeviceRemoved(Device &item)
    {
        EFC::ScopedPointer<Device> dev(m_topLevelDevices.take(item.id()));
        parent->slotDeviceRemoved(dev.get());
    }

    void slotDeviceChanged(const QDBusObjectPath &path)
    {
        QDBusInterface interface(QString::fromLatin1(UD_DBUS_SERVICE), path.path(), QString::fromLatin1(UD_DBUS_INTERFACE_DISKS_DEVICE), QDBusConnection::systemBus());

        if (interface.isValid())
            if (Device *device = m_allDevices.value(path.path()))
            {
                m_path = &path;
                m_interface = &interface;
                device->accept(*this);
            }
            else
            {
                EFC::ScopedPointer<Device> deviceHolder(processDevice(path.path(), interface));

                if (deviceHolder)
                {
                    m_allDevices[path.path()] = m_topLevelDevices[path.path()] = deviceHolder.get();
                    parent->slotDeviceAdded(deviceHolder.release());
                }
            }
    }

public: /* Visitor */
    virtual void visit(Drive &item)
    {
        QIcon icon;
        QString string = m_interface->property("DevicePresentationIconName").toString();

        if (!string.isEmpty())
        {
            using namespace LVFS;
            Interface::Holder res = Module::desktop().theme().icon(string.toUtf8());

            if (res.isValid())
            {
                QTextCodec *codec = QTextCodec::codecForLocale();
                icon.addFile(codec->toUnicode(res->as<IEntry>()->location()), QSize(Desktop::SmallIcon, Desktop::SmallIcon));
            }
        }

        if (icon.isNull())
        {
            using namespace LVFS;
            Interface::Holder res = Module::desktop().theme().icon(Desktop::Theme::Devices::HardDisk);

            if (res.isValid())
            {
                QTextCodec *codec = QTextCodec::codecForLocale();
                icon.addFile(codec->toUnicode(res->as<IEntry>()->location()), QSize(Desktop::SmallIcon, Desktop::SmallIcon));
            }
        }

        item.setIcon(icon);
        item.setLabel(driveLabel(*m_interface));
        item.setHidden(m_interface->property("DevicePresentationHide").toBool());
        item.setSize(m_interface->property("DeviceSize").toULongLong());

        parent->slotDeviceChanged(&item);
    }

    virtual void visit(HardDrive &item)
    {
        visit(static_cast<Drive &>(item));
    }

    virtual void visit(FlashDrive &item)
    {
        visit(static_cast<RemovableDrive &>(item));
        parent->slotDeviceChanged(&item);
    }

    virtual void visit(RemovableDrive &item)
    {
        QIcon icon;
        QString string = m_interface->property("DevicePresentationIconName").toString();

        if (!string.isEmpty())
        {
            using namespace LVFS;
            Interface::Holder res = Module::desktop().theme().icon(string.toUtf8());

            if (res.isValid())
            {
                QTextCodec *codec = QTextCodec::codecForLocale();
                icon.addFile(codec->toUnicode(res->as<IEntry>()->location()), QSize(Desktop::SmallIcon, Desktop::SmallIcon));
            }
        }

        if (!icon.isNull())
            item.setIcon(icon);

        item.setLabel(driveLabel(*m_interface));
        item.setHidden(m_interface->property("DevicePresentationHide").toBool());
        item.setSize(m_interface->property("DeviceSize").toULongLong());
        item.setMedia(Drive::stringToMediaType(m_interface->property("DriveMedia").toString()));
        item.setDetachable(m_interface->property("DriveCanDetach").toBool());
    }

    virtual void visit(OpticalDrive &item)
    {
        visit(static_cast<RemovableDrive &>(item));
        item.setEjectable(m_interface->property("DriveIsMediaEjectable").toBool());

        if (m_interface->property("DeviceIsMediaAvailable").toBool())
            if (item.partitions().isEmpty())
                opticalDrivePartition(&item, m_path->path(), *m_interface);
            else
            {
                Partition *partition = *item.partitions().constBegin();

                QStringList mountPaths(m_interface->property("DeviceMountPaths").toStringList());
                QString string = m_interface->property("DevicePresentationIconName").toString();
                QIcon icon;

                if (!string.isEmpty())
                {
                    using namespace LVFS;
                    Interface::Holder res = Module::desktop().theme().icon(string.toUtf8());

                    if (res.isValid())
                    {
                        QTextCodec *codec = QTextCodec::codecForLocale();
                        icon.addFile(codec->toUnicode(res->as<IEntry>()->location()), QSize(Desktop::SmallIcon, Desktop::SmallIcon));
                    }
                }

                if (icon.isNull())
                    icon = Drive::mediaTypeIcon(item.media());

                partition->setIcon(icon);
                partition->setLabel(partitionLabel(*m_interface, mountPaths));
                partition->setHidden(m_interface->property("DevicePresentationHide").toBool());
                partition->setNumber(m_interface->property("PartitionNumber").toInt());
                partition->setSize(m_interface->property("PartitionSize").toULongLong());
                partition->setMountPaths(mountPaths);

                parent->slotDeviceChanged(partition);
            }
        else
            if (!item.partitions().isEmpty())
            {
                EFC::ScopedPointer<Partition> partition(*item.partitions().constBegin());
                item.removePartition(partition->id());
                parent->slotDeviceRemoved(partition.get());
            }

        parent->slotDeviceChanged(&item);
    }

    virtual void visit(FloppyDrive &item)
    {
        visit(static_cast<RemovableDrive &>(item));
        parent->slotDeviceChanged(&item);
    }

    virtual void visit(Partition &item)
    {
        QIcon icon;
        QStringList mountPaths(m_interface->property("DeviceMountPaths").toStringList());
        QString string = m_interface->property("DevicePresentationIconName").toString();

        if (!string.isEmpty())
        {
            using namespace LVFS;
            Interface::Holder res = Module::desktop().theme().icon(string.toUtf8());

            if (res.isValid())
            {
                QTextCodec *codec = QTextCodec::codecForLocale();
                icon.addFile(codec->toUnicode(res->as<IEntry>()->location()), QSize(Desktop::SmallIcon, Desktop::SmallIcon));
            }
        }

        if (!icon.isNull())
            item.setIcon(icon);

        item.setLabel(partitionLabel(*m_interface, mountPaths));
        item.setHidden(m_interface->property("DevicePresentationHide").toBool());
        item.setNumber(m_interface->property("PartitionNumber").toInt());
        item.setSize(m_interface->property("PartitionSize").toULongLong());
        item.setMountPaths(mountPaths);

        parent->slotDeviceChanged(&item);
    }

public:
    Devices::Container m_topLevelDevices;
    Devices::Container m_allDevices;
    Devices *parent;
    QDBusInterface manager;

    const QDBusInterface *m_interface;
    const QDBusObjectPath *m_path;
};

#endif /* LDM_DEVICES_P_H_ */
