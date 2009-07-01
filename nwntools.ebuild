DESCRIPTION="NWN Tools"
HOMEPAGE="http://openknights.sf.net/"
SRC_URI="mirror://sourceforge/openknights/${P}.tar.bz2"
LICENSE="OpenKnights"
SLOT="0"
KEYWORDS="~x86"
IUSE=""
DEPEND=""

src_install ()
{
	einstall || die
	dodoc AUTHORS COPYING ChangeLog INSTALL NEWS README* THANKS
}

