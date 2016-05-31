#include "cbookmark.h"
#include "assert/advanced_assert.h"

DISABLE_COMPILER_WARNINGS
#include <QStringBuilder>
#include <QStringList>
RESTORE_COMPILER_WARNINGS

CBookmark::CBookmark(const QString & serializedBookmark)
{
	const QStringList components = serializedBookmark.split(';');
	assert_and_return_r(components.size() == 2, );

	filePath = components[0];
	wordIndex = (size_t)components[1].toULongLong();
}

CBookmark::CBookmark(const QString& path, const size_t position) : filePath(path), wordIndex(position)
{
}

QString CBookmark::toString() const
{
	return filePath % ';' % QString::number(wordIndex);
}
