TEMPLATE = subdirs

SUBDIRS += app cpputils core qtutils cpp-template-utils

core.depends = cpputils qtutils

qtutils.depends = cpputils

app.depends = core

!android*{
	SUBDIRS += autoupdater
	autoupdater.subdir = github-releases-autoupdater
	autoupdater.depends = cpputils

	app.depends += autoupdater
}
