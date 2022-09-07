Name:           feather
Version:        1.0
Release:        1
Summary:        The only low memory text editor.
Group:          Applications/TextEditors
License:        Proprietary
URL:            https://www.feathereditor.com/
Vendor:         MateuszWojtczak
Source:         https://feathereditor.com/feather-1.0-1.x86_64.tar.gz
Prefix:         %{_prefix}
Packager:       MateuszWojtczak <0xbaca@gmail.com>
Requires:       ncurses-devel >= 6.1

%define debug_package %{nil}

%description
The only terminal based text editor designed to work with BIG files.
It doesn't matter if you will edit a 100MB or 100GB file. Feather will do.

%prep
%setup -q -n %{name}-%{version}

%build
make

%install
make BUILDROOT="../../BUILDROOT" install_rpm_x86_64

%post
gunzip /usr/share/man/man1/feather.1.gz

%files
%defattr(-,root,root)
%license LICENSE
/usr/local/bin/feather
/usr/share/man/man1/feather.1.gz
/*
