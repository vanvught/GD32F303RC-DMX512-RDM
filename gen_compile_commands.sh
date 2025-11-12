#!/bin/bash

PROJECT_DIR=$(pwd)

INCLUDE_PATHS=$(find . -name '*.h' \
  -not -path '*/venv/*' -not -path '*/.git/*' -not -path '*/build/*' \
  -exec dirname {} \; | sort -u | awk '{
    keep = 1
    for (i in seen) {
      if (index($0, seen[i]) == 1 && length($0) > length(seen[i]) && substr($0, length(seen[i])+1, 1) == "/") {
        keep = 0
        break
      }
      if (index(seen[i], $0) == 1 && length(seen[i]) > length($0) && substr(seen[i], length($0)+1, 1) == "/") {
        delete seen[i]
      }
    }
    if (keep) seen[length(seen)+1]=$0
  } END {
    for (i in seen) print "-I" seen[i]
  }' | tr '\n' ' ')

# Add paths to freestanding headers for toolchain
INCLUDE_PATHS="$INCLUDE_PATHS -I/Users/arjanvanvught/Library/xPacks/@xpack-dev-tools/arm-none-eabi-gcc/14.2.1-1.1.1/.content/arm-none-eabi/include/c++/14.2.1"
INCLUDE_PATHS="$INCLUDE_PATHS -I/Users/arjanvanvught/Library/xPacks/@xpack-dev-tools/arm-none-eabi-gcc/14.2.1-1.1.1/.content/arm-none-eabi/include/c++/14.2.1/arm-none-eabi/thumb/nofp"

DEFINES="-DCONFIG_DMX_DOUBLE_INPUT_BUFFER -DGD32 -DGD32F30X -DGD32F30X_HD -DGD32F303RC -DBOARD_GD32F303RC -DCONFIG_STORE_USE_ROM -DNO_EMAC -DCONFIG_HAL_USE_SYSTICK -DRDM_RESPONDER -DCONFIG_DISPLAY_FIX_FLIP_VERTICALLY -DCONFIG_RDM_ENABLE_SELF_TEST -DCONFIG_RDM_ENABLE_MANUFACTURER_PIDS -DCONFIG_DMXNODE_PIXEL_MAX_PORTS=1 -DOUTPUT_DMX_PIXEL -DDISPLAY_UDF -DCONFIG_STORE_USE_ROM -DDISABLE_FS -DCONFIG_STORE_USE_ROM -DNO_EMAC -D_TIME_STAMP_YEAR_=2025 -D_TIME_STAMP_MONTH_=7 -D_TIME_STAMP_DAY_=5 -DCONFIG_HAL_USE_SYSTICK"

echo $INCLUDE_PATHS
echo $DEFINES

# Start the JSON array
echo "[" > compile_commands.json

# Find all .cpp files and create a JSON entry for each
find . -name '*.cpp' | while read file; do
  echo "  {" >> compile_commands.json
  echo "    \"directory\": \"${PROJECT_DIR}\"," >> compile_commands.json
  echo "    \"command\": \"/Users/arjanvanvught/arm-none-eabi-gcc/bin/arm-none-eabi-g++ -std=c++20 -ffreestanding -nostdlib -nostdinc++ $INCLUDE_PATHS $DEFINES $file\"," >> compile_commands.json
  echo "    \"file\": \"$file\"" >> compile_commands.json
  echo "  }," >> compile_commands.json
done

# Remove last comma and close JSON array
sed -i '' -e '$ s/},/}/' compile_commands.json
echo "]" >> compile_commands.json

jq . compile_commands.json > tmp.json && mv tmp.json compile_commands.json
