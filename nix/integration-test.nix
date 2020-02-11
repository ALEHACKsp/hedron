{ expect, nova, qemu, grub2, mtools, OVMF, xorriso, stdenv }:
let
  grub_image = "grub_image.iso";
in
stdenv.mkDerivation {
  name = "nova-integration-tests";
  inherit (nova) src;

  buildInputs = [ expect qemu grub2 mtools OVMF xorriso ];

  postPatch = ''
    patchShebangs test/integration/qemu-boot
    patchShebangs tools/gen_usb.sh
  '';

  buildPhase = ''
    test/integration/qemu-boot ${nova}/hypervisor.elf32 | tee output.log
    tools/gen_usb.sh ${grub_image} ${nova}/hypervisor.elf32 tools/grub.cfg.tmpl
    test/integration/qemu-boot ${grub_image} -i | tee -a output.log
    test/integration/qemu-boot ${grub_image} -i -e ${OVMF.fd}/FV | tee -a output.log
  '';

  installPhase = ''
    mkdir -p $out/nix-support
    cp output.log $out/
    echo "report testlog $out output.log" > $out/nix-support/hydra-build-products
  '';
}
