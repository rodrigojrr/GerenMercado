#!/usr/bin/env bash
# Exporta slides.html para slides.pdf tirando um screenshot por slide.
# Requer: chromium (ou google-chrome) + ImageMagick (magick/convert).
#
# A URL usa "?capture": nesse modo o slides.html mostra todos os fragments
# de uma vez, então o PDF sai com o código completo e sem as etapas de
# animação abertas pela metade.
set -euo pipefail

cd "$(dirname "$0")"

CHROME="${CHROME:-$(command -v chromium || command -v chromium-browser || command -v google-chrome)}"
TOTAL=17
TMP="$(mktemp -d)"
trap 'rm -rf "$TMP"' EXIT

for i in $(seq 0 $((TOTAL - 1))); do
  printf 'slide %2d/%d\n' "$((i + 1))" "$TOTAL"
  "$CHROME" --headless=old --no-sandbox --disable-gpu --hide-scrollbars \
    --force-device-scale-factor=2 --window-size=1280,720 \
    --virtual-time-budget=6000 \
    --screenshot="$TMP/$(printf 's%02d' $((i + 1))).png" \
    "file://$PWD/slides.html?capture#/$i" >/dev/null 2>&1
done

if command -v magick >/dev/null 2>&1; then
  magick "$TMP"/s*.png -units PixelsPerInch -density 192 slides.pdf
else
  convert "$TMP"/s*.png -units PixelsPerInch -density 192 slides.pdf
fi

echo "gerado: $PWD/slides.pdf"
