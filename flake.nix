{
  description = "ld.so hack allowing Nix binaries to impurely load RHEL system libraries as last resort";

  inputs.flox-floxpkgs.url = "github:flox/floxpkgs";

  outputs = args @ {flox-floxpkgs, ...}: flox-floxpkgs.project args (_: {});
}
