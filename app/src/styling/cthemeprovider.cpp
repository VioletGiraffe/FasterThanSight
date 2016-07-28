#include "cthemeprovider.h"

CThemeProvider& CThemeProvider::instance()
{
	static CThemeProvider inst;
	return inst;
}

CThemeProvider::CThemeProvider()
{

}
