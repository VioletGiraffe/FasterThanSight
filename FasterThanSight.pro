TEMPLATE = subdirs

SUBDIRS += app autoupdater cpputils core qtutils

cpputils.subdir = cpputils

autoupdater.subdir = github-releases-autoupdater
autoupdater.depends = cpputils

core.subdir = core
core.depends = cpputils qtutils

qtutils.subdir = qtutils
qtutils.depends = cpputils

app.subdir  = app
app.depends = core autoupdater
