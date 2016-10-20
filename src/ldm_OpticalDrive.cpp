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

#include "ldm_OpticalDrive.h"
#include "ldm_Visitor.h"


namespace LDM {

OpticalDrive::OpticalDrive(const Id &id,
	  	 	 	 	 	   const QIcon &icon,
	  	 	 	 	 	   const QString &label,
	  	 	 	 	 	   bool hidden,
	  	 	 	 	 	   Device *parent,
	  	 	 	 	 	   quint64 size,
	  	 	 	 	 	   MediaTypeSet mediaCompatibility,
	  	 	 	 	 	   MediaType media,
	  	 	 	 	 	   bool detachable,
	  	 	 	 	 	   bool ejectable) :
	RemovableDrive(id, icon, label, hidden, parent, size, mediaCompatibility, media, detachable),
	m_ejectable(ejectable)
{}

const OpticalDrive::MediaTypeSet &OpticalDrive::mediaTypeSet()
{
	static MediaTypeSet set = MediaTypeSet::fromList(
			MediaTypeList() << Optical
							<< Optical_Cd
							<< Optical_Cd_R
							<< Optical_Cd_Rw
							<< Optical_Dvd
							<< Optical_Dvd_R
							<< Optical_Dvd_Rw
							<< Optical_Dvd_Ram
							<< Optical_Dvd_Plus_R
							<< Optical_Dvd_Plus_Rw
							<< Optical_Dvd_Plus_R_Dl
							<< Optical_Dvd_Plus_Rw_Dl
							<< Optical_Bd
							<< Optical_Bd_R
							<< Optical_Bd_Re
							<< Optical_HdDvd
							<< Optical_HdDvd_R
							<< Optical_HdDvd_Rw
							<< Optical_Mo
							<< Optical_Mrw
							<< Optical_Mrw_W);
	return set;
}

void OpticalDrive::accept(Visitor &visitor)
{
    visitor.visit(*this);
}

}
