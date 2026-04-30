Name:       it.mardy.Chromono
Summary:    Circle color puzzle game
Version:    1.1.3
Release:    1
License:    GPLv2
URL:        https://auroraos.ru
Source0:    %{name}-%{version}.tar.bz2

BuildRequires: pkgconfig(egl)
BuildRequires: pkgconfig(glesv2)
BuildRequires: pkgconfig(sdl2)
BuildRequires: pkgconfig(vorbisfile)

%description
All those half-colored spheres could really need some help. And with that, we obviously mean your help, otherwise this would probably not be a game, but a movie or something. Grab those fully-colored spheres and share the color (and the correct one at that) to bring the world order back to normal.

%prep
%autosetup

%build
%cmake -DCMAKE_CXX_FLAGS=-DCONFIG_REL_PATH='\"it.mardy/Chromono\"' -DUSE_OPENGL_ES=ON -GNinja
%ninja_build

%install
%ninja_install
mv %{buildroot}/%{_bindir}/chromono %{buildroot}/%{_bindir}/%{name}
rm -f %{buildroot}/%{_datadir}/applications/chromono.desktop
rm -rf %{buildroot}/%{_datadir}/pixmaps
cp rpm/%{name}.desktop %{buildroot}/%{_datadir}/applications/
for icondir in rpm/icons/*
do
    RES=$(basename $icondir)
    DEST=%{buildroot}/%{_datadir}/icons/hicolor/$RES/apps
    mkdir -p "$DEST"
    cp -a rpm/icons/$RES/* "$DEST/"
done
cp rpm/manifest.json %{buildroot}/

%files
%defattr(-,root,root,-)
%{_bindir}/%{name}
%defattr(644,root,root,-)
%{_datadir}/applications/%{name}.desktop
%{_datadir}/icons/hicolor/*/apps/%{name}.png
/manifest.json
