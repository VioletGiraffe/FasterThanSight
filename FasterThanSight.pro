TEMPLATE = subdirs

SUBDIRS += app autoupdater cpputils core qtutils cpp-template-utils

core.depends = cpputils qtutils

qtutils.depends = cpputils

autoupdater.subdir = github-releases-autoupdater
autoupdater.depends = cpputils

app.depends = core autoupdater
