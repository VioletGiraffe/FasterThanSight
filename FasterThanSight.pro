TEMPLATE = subdirs

SUBDIRS += app autoupdater cpputils core qtutils cpp-template-utils

autoupdater.subdir = github-releases-autoupdater
autoupdater.depends = cpputils

core.depends = cpputils qtutils

qtutils.depends = cpputils

app.depends = core autoupdater
