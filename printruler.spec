# norootforbuild

%define jobs $(( `/usr/bin/getconf _NPROCESSORS_ONLN` + 1 ))

Name:       printruler
Version:    r12
Release:    1.0
License:    GNU GPL v3
Group:      Hardware/Printing
Url:        http://code.google.com/p/printruler/
Source:     %{name}-%{version}.tar.bz2
BuildRoot:  %{_tmppath}/%{name}-%{version}-build
BuildRequires:  fdupes libqt4-devel update-desktop-files
Requires:   ghostscript-library psutils xpdf-tools
Summary:    Application similar to fineprint, but for linux

%description
Printruler lets you merge multiple print jobs, edit them (delete pages, add blank pages) and do other cool stuff.


%prep
%setup -qn %{name}

%build
#qmake QMAKE_CXXFLAGS="$RPM_OPT_FLAGS" QMAKE_STRIP=echo
qmake QMAKE_CXXFLAGS="$RPM_OPT_FLAGS" CONFIG-=debug_and_release CONFIG+=debug
%__make %{?jobs:-j%jobs}
%suse_update_desktop_file -c printruler Printruler "" printruler printruler Utility PrintingUtility

%install
%__make INSTALL_ROOT="%{buildroot}" install
%__install -Dm 0644 logo.png %{buildroot}%{_datadir}/pixmaps/printruler.png
%fdupes %{buildroot}

%clean
test "%{buildroot}" != "/" && %__rm -rf %{buildroot}

%files
%defattr(-,root,root)
%doc README
%{_bindir}/printruler
%{_datadir}/applications/printruler.desktop
%{_datadir}/pixmaps/printruler.png
%{_datadir}/printruler

%changelog
* Sun Nov 06 2008 noMail@please.com
- Initial RPM
