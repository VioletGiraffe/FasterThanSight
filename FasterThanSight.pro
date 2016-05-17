TEMPLATE = subdirs

SUBDIRS += autoupdater app cpputils

cpputils.subdir = cpputils

autoupdater.subdir = github-releases-autoupdater
autoupdater.depends = cpputils

app.subdir  = app
app.depends = autoupdater