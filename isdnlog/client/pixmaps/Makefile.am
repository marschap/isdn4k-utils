data_DATA = isdnlog.xpm

datadir = $(prefix)/share/apps/@PACKAGE@/pics

install-data-local:
	$(mkinstalldirs) $(appsdir)
	$(INSTALL_DATA) @PACKAGE@.xpm $(kde_icondir)/@PACKAGE@.xpm
	$(INSTALL_DATA) isdnlog.xpm $(kde_datadir)/@PACKAGE@/isdnlog.xpm

uninstall-local:
	rm -f $(kde_icondir)/@PACKAGE@.xpm
	rm -f $(kde_datadir)/@PACKAGE@/isdnlog.xpm
