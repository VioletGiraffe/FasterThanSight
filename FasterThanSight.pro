TEMPLATE = subdirs

SUBDIRS += autoupdater app cpputils core

cpputils.subdir = cpputils

autoupdater.subdir = github-releases-autoupdater
autoupdater.depends = cpputils

core.subdir = core
core.depends = cpputils

app.subdir  = app
app.depends = core autoupdater
