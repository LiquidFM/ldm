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

#ifndef LDM_FLOPPYDRIVE_H_
#define LDM_FLOPPYDRIVE_H_

#include <ldm/RemovableDrive>


namespace LDM {

class PLATFORM_MAKE_PUBLIC FloppyDrive : public RemovableDrive
{
public:
	FloppyDrive(const Id &id,
		  	    const QIcon &icon,
		  	    const QString &label,
		  	    bool hidden,
		  	    Device *parent,
		  	    quint64 size,
		  	    MediaTypeSet mediaCompatibility,
		  	    MediaType media,
		  	    bool detachable);

	static const MediaTypeSet &mediaTypeSet();
    virtual void accept(Visitor &visitor);
};

}

#endif /* LDM_FLOPPYDRIVE_H_ */
