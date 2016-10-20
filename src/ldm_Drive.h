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

#ifndef LDM_DRIVE_H_
#define LDM_DRIVE_H_

#include <QtCore/QMap>
#include <QtCore/QSet>
#include <QtCore/QList>
#include <QtCore/QStringList>
#include <ldm/Device>


namespace LDM {

class Partition;

/**
 * http://hal.freedesktop.org/docs/udisks/Device.html
 */
class PLATFORM_MAKE_PUBLIC Drive : public Device
{
public:
	typedef QMap<Id, Partition *> Container;

	enum MediaType
	{
		Flash, // Flash Card
		Flash_CompactFlash, // CompactFlash
		Flash_MemoryStick, // MemoryStick
		Flash_SmartMedia, // SmartMedia
		Flash_SecureDigital, // Secure Digital
		Flash_SecureDigitalHighCapacity, // Secure Digital High-Capacity
		Flash_MultiMediaCard, // MultiMediaCard
		Floppy, // Floppy Disk
		Floppy_Zip, // Zip Disk
		Floppy_Jaz, // Jaz Disk
		Optical, // Optical Disc
		Optical_Cd, // Compact Disc
		Optical_Cd_R, // Compact Disc Recordable
		Optical_Cd_Rw, // Compact Disc Rewritable
		Optical_Dvd, // Digital Versatile Disc
		Optical_Dvd_R, // DVD-R
		Optical_Dvd_Rw, // DVD-RW
		Optical_Dvd_Ram, // DVD-RAM
		Optical_Dvd_Plus_R, // DVD+R
		Optical_Dvd_Plus_Rw, // DVD+RW
		Optical_Dvd_Plus_R_Dl, // DVD+R Dual Layer
		Optical_Dvd_Plus_Rw_Dl, // DVD+RW Dual Layer
		Optical_Bd, // Bluray Disc
		Optical_Bd_R, // BluRay Recordable
		Optical_Bd_Re, // BluRay Rewritable
		Optical_HdDvd, // HD DVD
		Optical_HdDvd_R, // HD DVD Recordable
		Optical_HdDvd_Rw, // HD DVD Rewritable
		Optical_Mo, // Magneto Optical
		Optical_Mrw, // Can read Mount Rainer media
		Optical_Mrw_W, // Can write Mount Rainer media
		NoMedia
	};

	typedef QSet<MediaType>  MediaTypeSet;
	typedef QList<MediaType> MediaTypeList;

	enum ConnectionInterface
	{
		Virtual, // Device is a composite device e.g. Software RAID or similar
		Ata, // Connected via ATA
		Ata_Serial, // Connected via Serial ATA
		Ata_Serial_eSata, // Connected via eSATA
		Ata_Parallel, // Connected via Parallel ATA
		Scsi, // Connected via SCSI
		Usb, // Connected via the Universal Serial Bus
		Firewire, // Connected via Firewire
		Sdio, // Connected via SDIO
		Platform, // Part of the platform, e.g. PC floppy drive
		Unknown
	};

	struct Details
	{
		QString vendor;
		QString model;
		QString revision;
		QString serial;
		QString wwn;
		ConnectionInterface interface;
		quint32 connectionSpeed; // in bits per second
	};

public:
	Drive(const Id &id,
		  const QIcon &icon,
		  const QString &label,
		  bool hidden,
		  Device *parent,
		  quint64 size);
	virtual ~Drive();

	quint64 size() const { return m_size; }
	void setSize(quint64 size) { m_size = size; }

	const Container &partitions() const { return m_partitions; }

	void addPartition(Partition *partition);
	void removePartition(const Id &id);
	Partition *takePartition(const Id &id);

    virtual void accept(Visitor &visitor);

	static QIcon mediaTypeIcon(MediaType mediaType);
	static MediaType stringToMediaType(const QString &media);
	static MediaTypeSet stringListToMediaTypeSet(const QStringList &media);

private:
	quint64 m_size;
	Container m_partitions;
};

}

#endif /* LDM_DRIVE_H_ */
