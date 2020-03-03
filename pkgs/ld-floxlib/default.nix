{ self, lib, buildEnv, stdenv }:

let
  pname = "ld-floxlib";
  ld_floxlib_libs = buildEnv {
    name = "${pname}-libs";
    paths = [
      # List here any libraries that you would prefer to see
      # loaded purely from Nix.
      stdenv.cc.cc.lib	# for libstdc++.so.6
    ];
  };

in
stdenv.mkDerivation {
  inherit pname;
  version = "0.0.0-${lib.flox-floxpkgs.getRev self}";
  src = self;
  buildInputs = [ ld_floxlib_libs ];
  makeFlags = [
    "PREFIX=$(out)"
    "CFLAGS=-DLD_FLOXLIB_LIB='\"${ld_floxlib_libs}/lib\"'"
  ];
  meta.description = "ld.so hack allowing Nix binaries to impurely load RHEL system libraries as last resort";
}
