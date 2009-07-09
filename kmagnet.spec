#
# spec file for package kmagnet (Version 0.01beta)
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
Summary:        A kde4 simple puzzle game
Version:        0.01beta
Release:        1
URL:            http://personal.telefonica.terra.es/web/oscarmartinez/v2/
BuildRoot:      %{_tmppath}/%{name}-%{version}-build
Source0:        %name-%{version}.tar.bz2

%description
A simple puzzle game.


Authors:
--------
    Oscar Martinez <omllobet@gmail.com>

%prep
%setup -q

%build  
  %cmake_kde4 -d build  
  %make_jobs  

%install
  cd build  
  %makeinstall  
  cd ..  

%clean
rm -rf $RPM_BUILD_ROOT
rm -rf filelists 

%files
%defattr(-,root,root)
%doc AUTHORS COPYING ChangeLog README TODO
/usr/bin/kmagnet
/usr/share/applications/kde4/kmagnet.desktop
/usr/share/kde4/apps/kmagnet

%changelog
* Sun Apr  5 2009 - omllobet@gmail.com
- initial package created
