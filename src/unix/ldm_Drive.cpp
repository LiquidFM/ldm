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

#include "../ldm_Drive.h"
#include "../ldm_Partition.h"
#include "../ldm_Visitor.h"

#include <lvfs/Module>
#include <lvfs/IEntry>

#include <QtCore/QTextCodec>


namespace LDM {

Drive::Drive(const Id &id,
		  	 const QIcon &icon,
		  	 const QString &label,
		  	 bool hidden,
		  	 Device *parent,
		  	 quint64 size) :
	Device(id, icon, label, hidden, parent),
	m_size(size)
{}

Drive::~Drive()
{
	qDeleteAll(m_partitions);
}

void Drive::addPartition(Partition *partition)
{
	m_partitions.insert(partition->id(), partition);
}

void Drive::removePartition(const Id &id)
{
	m_partitions.remove(id);
}

Partition *Drive::takePartition(const Id &id)
{
	return m_partitions.take(id);
}

void Drive::accept(Visitor &visitor)
{
    visitor.visit(*this);
}

QIcon Drive::mediaTypeIcon(MediaType mediaType)
{
    using namespace LVFS;
    Interface::Holder res;
    QIcon icon;

	switch (mediaType)
	{
		case Flash:
		case Flash_CompactFlash:
			res = Module::desktop().theme().icon(Desktop::Theme::Devices::Flash);
			break;

		case Flash_MemoryStick:
		    res = Module::desktop().theme().icon(Desktop::Theme::Devices::FlashMemoryStick);
            break;

		case Flash_SmartMedia:
            res = Module::desktop().theme().icon(Desktop::Theme::Devices::FlashSmartMedia);
            break;

		case Flash_SecureDigital:
		case Flash_SecureDigitalHighCapacity:
		case Flash_MultiMediaCard:
            res = Module::desktop().theme().icon(Desktop::Theme::Devices::FlashSdMmc);
            break;

		case Floppy:
		case Floppy_Zip:
		case Floppy_Jaz:
            res = Module::desktop().theme().icon(Desktop::Theme::Devices::Floppy);
            break;

		case Optical:
		case Optical_Cd:
		case Optical_Cd_R:
		case Optical_Cd_Rw:
            res = Module::desktop().theme().icon(Desktop::Theme::Devices::Optical);
            break;

		case Optical_Dvd:
		case Optical_Dvd_R:
		case Optical_Dvd_Rw:
		case Optical_Dvd_Ram:
		case Optical_Dvd_Plus_R:
		case Optical_Dvd_Plus_Rw:
		case Optical_Dvd_Plus_R_Dl:
		case Optical_Dvd_Plus_Rw_Dl:
            res = Module::desktop().theme().icon(Desktop::Theme::Devices::OpticalDvd);
            break;

		case Optical_Bd:
		case Optical_Bd_R:
		case Optical_Bd_Re:
		case Optical_HdDvd:
		case Optical_HdDvd_R:
		case Optical_HdDvd_Rw:
            res = Module::desktop().theme().icon(Desktop::Theme::Devices::OpticalBluRay);
            break;

		case Optical_Mo:
		case Optical_Mrw:
		case Optical_Mrw_W:
            res = Module::desktop().theme().icon(Desktop::Theme::Devices::Optical);
            break;

		default:
            break;
	}

	if (res.isValid())
	{
        QTextCodec *codec = QTextCodec::codecForName(Module::desktop().locale().encoding());
        icon.addFile(codec->toUnicode(res->as<IEntry>()->location()), QSize(Desktop::SmallIcon, Desktop::SmallIcon));
	}

	return icon;
}

Drive::MediaType Drive::stringToMediaType(const QString &media)
{
	static const QString types[NoMedia] =
	{
		QString::fromLatin1("flash"),
		QString::fromLatin1("flash_cf"),
		QString::fromLatin1("flash_ms"),
		QString::fromLatin1("flash_sm"),
		QString::fromLatin1("flash_sd"),
		QString::fromLatin1("flash_sdhc"),
		QString::fromLatin1("flash_mmc"),
		QString::fromLatin1("floppy"),
		QString::fromLatin1("floppy_zip"),
		QString::fromLatin1("floppy_jaz"),
		QString::fromLatin1("optical"),
		QString::fromLatin1("optical_cd"),
		QString::fromLatin1("optical_cd_r"),
		QString::fromLatin1("optical_cd_rw"),
		QString::fromLatin1("optical_dvd"),
		QString::fromLatin1("optical_dvd_r"),
		QString::fromLatin1("optical_dvd_rw"),
		QString::fromLatin1("optical_dvd_ram"),
		QString::fromLatin1("optical_dvd_plus_r"),
		QString::fromLatin1("optical_dvd_plus_rw"),
		QString::fromLatin1("optical_dvd_plus_r_dl"),
		QString::fromLatin1("optical_dvd_plus_rw_dl"),
		QString::fromLatin1("optical_bd"),
		QString::fromLatin1("optical_bd_r"),
		QString::fromLatin1("optical_bd_re"),
		QString::fromLatin1("optical_hddvd"),
		QString::fromLatin1("optical_hddvd_r"),
		QString::fromLatin1("optical_hddvd_rw"),
		QString::fromLatin1("optical_mo"),
		QString::fromLatin1("optical_mrw"),
		QString::fromLatin1("optical_mrw_w")
	};

	for (int i = Flash; i < NoMedia; ++i)
		if (media == types[i])
			return static_cast<MediaType>(i);

	return NoMedia;
}

Drive::MediaTypeSet Drive::stringListToMediaTypeSet(const QStringList &media)
{
	Drive::MediaTypeSet res;

	for (QStringList::size_type i = 0, size = media.size(); i < size; ++i)
		res.insert(stringToMediaType(media.at(i)));

	return res;
}

}
