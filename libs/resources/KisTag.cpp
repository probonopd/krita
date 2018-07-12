/*
 * Copyright (C) 2018 Boudewijn Rempt <boud@valdyas.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "KisTag.h"

#include <QIODevice>
#include <QLocale>
#include <QBuffer>
#include <QByteArray>
#include <QStandardPaths>
#include <QFile>

#include "kconfigini_p.h"
#include "kconfigbackend_p.h"
#include "kconfigdata.h"

#include <kis_debug.h>

const QByteArray KisTag::s_group {"Desktop Entry"};
const QByteArray KisTag::s_type {"Type"};
const QByteArray KisTag::s_tag {"Tag"};
const QByteArray KisTag::s_name {"Name"};
const QByteArray KisTag::s_url {"URL"};
const QByteArray KisTag::s_comment {"Comment"};

class KisTag::Private {
public:
    QString url;
    QString name;
    QString comment;
    KEntryMap map;
};

KisTag::KisTag()
    : d(new Private)
{
}

KisTag::~KisTag()
{

}

QString KisTag::name() const
{
    return d->name;
}

void KisTag::setName(const QString &name) const
{
    d->map.setEntry(s_group, s_name, name, KEntryMap::EntryDirty);
    d->name = name;
}

QString KisTag::url() const
{
    return d->url;
}

void KisTag::setUrl(const QString &url) const
{
    d->map.setEntry(s_group, s_url, url, KEntryMap::EntryDirty);
    d->url = url;
}

QString KisTag::comment() const
{
    return d->comment;
}

void KisTag::setComment(const QString &comment) const
{
    d->map.setEntry(s_group, s_comment, comment, KEntryMap::EntryDirty);
    d->comment = comment;
}

bool KisTag::load(QIODevice &io)
{
    if (!io.isOpen()) {
        io.open(QIODevice::ReadOnly);
    }
    KIS_ASSERT(io.isOpen());

    KConfigIniBackend ini;
    KConfigBackend::ParseInfo r = ini.parseConfigIO(io, QLocale().name().toUtf8(), d->map, KConfigBackend::ParseOption::ParseGlobal, false);
    if (r != KConfigBackend::ParseInfo::ParseOk) {
        qWarning() << "Could not load this tag file" << r;
        return false;
    }

    QString t = d->map.getEntry(s_group, s_type);
    if (t != s_tag) {
        qWarning() << "Not a tag desktop file" << t;
        return false;
    }

    d->url = d->map.getEntry(s_group, s_url);
    d->name = d->map.getEntry(s_group, s_name, QString(), KEntryMap::SearchLocalized);
    d->comment = d->map.getEntry(s_group, s_comment, QString(), KEntryMap::SearchLocalized);

    return true;
}

bool KisTag::save(QIODevice &io)
{
    KConfigIniBackend ini;
    ini.writeEntries(QLocale().name().toUtf8(), io, d->map);
    return true;
}
