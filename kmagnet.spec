#
# spec file for package kMagnet
#
# Copyright (c) 2009 Oscar Martinez
# This file and all modifications and additions to the pristine
# package are under the same license as the package itself.
#

# norootforbuild

Name:           kmagnet
BuildRequires:  libkdegames4-devel libkde4-devel
License:        GNU General Public License (GPL)
Group:          Amusements/Games/Board/Puzzle
Summary:        A simple puzzle game
Version:        0.03
Release:        1
URL:            http://personal.telefonica.terra.es/web/oscarmartinez/v2/
BuildRoot:      %{_tmppath}/%{name}-%{version}-build
Source0:        %name-%{version}.tar.bz2


%description
A simple puzzle game. Basically you can move the ball up down left or right and it stops when it finds an obstacle. To win the ball has to arrive to the hole. 


Authors:
--------
    Oscar Martinez <omllobet@gmail.com>

%debug_package
%prep
%setup -q

%build  
  %cmake_kde4 -d build  
  %make_jobs  

%install
  cd build  
  %makeinstall  
  cd ..  
  %suse_update_desktop_file -r kmagnet          Game PuzzleGame

%clean
rm -rf $RPM_BUILD_ROOT
rm -rf filelists 

%files
%defattr(-,root,root)
%doc AUTHORS COPYING ChangeLog README TODO
/usr/bin/kmagnet
/usr/share/applications/kde4/kmagnet.desktop
/usr/share/doc/kde/HTML/en/kmagnet/screenshot.png
/usr/share/doc/kde/HTML/en/kmagnet/common
/usr/share/doc/kde/HTML/en/kmagnet/index.cache.bz2
/usr/share/doc/kde/HTML/en/kmagnet/index.docbook
/usr/share/icons/hicolor/16x16/apps/kMagnet.png
/usr/share/icons/hicolor/32x32/apps/kMagnet.png
/usr/share/icons/hicolor/48x48/apps/kMagnet.png
/usr/share/kde4/config.kcfg/kmagnet.kcfg
/usr/share/kde4/apps/kMagnet/data/puzzle1.kmp
/usr/share/kde4/apps/kMagnet/data/puzzle2.kmp
/usr/share/kde4/apps/kMagnet/data/puzzle3.kmp
/usr/share/kde4/apps/kMagnet/images/final.png
/usr/share/kde4/apps/kMagnet/images/free.png
/usr/share/kde4/apps/kMagnet/images/notfree.png
/usr/share/kde4/apps/kMagnet/kMagnetui.rc
%dir /usr/share/kde4/apps/kMagnet
%dir /usr/share/kde4/apps/kMagnet/data
%dir /usr/share/kde4/apps/kMagnet/images
%dir /usr/share/doc/kde/HTML/en/kmagnet

%changelog
* Sun Oct 11 2009 - package updated
* Sun Apr  5 2009 - omllobet@gmail.com
- initial package created
  
  