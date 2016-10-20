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

#ifndef LDM_DEVICE_H_
#define LDM_DEVICE_H_

#include <platform/utils.h>
#include <QtCore/QString>
#include <QtGui/QIcon>


namespace LDM {

class Visitor;

class PLATFORM_MAKE_PUBLIC Device
{
public:
#if PLATFORM_OS(UNIX)
	typedef QString Id;
#else
#error Unsupported OS!
#endif

public:
	Device(const Id &id,
		   const QIcon &icon,
		   const QString &label,
		   bool hidden,
		   Device *parent);
	virtual ~Device();

    const Id &id() const { return m_id; }

    const QIcon &icon() const { return m_icon; }
	void setIcon(const QIcon &icon) { m_icon = icon; }

	const QString &label() const { return m_label; }
	void setLabel(const QString &label) { m_label = label; }

	bool isHidden() const { return m_hidden; }
	void setHidden(bool hidden) { m_hidden = hidden; }

	Device *parent() const { return m_parent; }

	virtual void accept(Visitor &visitor) = 0;

    template <typename R> inline
    const R *as() const { return static_cast<const R *>(this); }

    template <typename R> inline
    R *as() { return static_cast<R *>(this); }

private:
    Id m_id;
	QIcon m_icon;
	QString m_label;
	bool m_hidden;
	Device *m_parent;
};

}

#endif /* LDM_DEVICE_H_ */
