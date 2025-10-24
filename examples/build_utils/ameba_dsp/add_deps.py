import xml.etree.ElementTree as ET
import argparse

def insert_base_entries_to_xml(file_path, entries):
    # Parse the XML file
    tree = ET.parse(file_path)
    root = tree.getroot()

    # Iterate over the entries to add
    for entry in entries:
        path, key_name, list_entry_value = entry

        # Find the specified <key>
        target_entry = root.find(f".//{path}/[key='{key_name}']")
        if target_entry is None:
            print(f"Target <{path}> with <key>{key_name}</key> not found. Skipping...")
            continue
        else:
            value_element = target_entry.find("value")
            if value_element is None:
                print(f"<value> element not found inside for key '{key_name}'. Skipping...")
                continue

        # Add a new <ListEntry> element
        new_entry = ET.SubElement(value_element, "ListEntry")
        if isinstance(list_entry_value, dict):  # Handle key-value pairs for Defines
            for k, v in list_entry_value.items():
                new_entry.set(k, v)
        else:  # Handle plain text values for Includes, Libraries, etc.
            new_entry.text = list_entry_value

    # Write the updated XML back to the file
    tree.write(file_path, encoding="utf-8", xml_declaration=True)
    print("Insert Base Settings successfully.")

def add_overridden_settings_to_xml(file_path, overridden_xml_path):
    # Parse the XML file
    tree = ET.parse(file_path)
    root = tree.getroot()
    add_root = ET.parse(overridden_xml_path).getroot()
    
    build_settings_entry = root.find(".//BuildSettings")
    build_settings_entry.append(add_root)

    # Write the updated XML back to the file
    tree.write(file_path, encoding="utf-8", xml_declaration=True)
    print("Add Overridden Settings successfully.")

if __name__ == "__main__":
    # Parse command-line arguments
    parser = argparse.ArgumentParser(description="Insert dependencies into Release.bts files.")
    parser.add_argument("--project_dir", help="Path to the project_dir." ,required=True)
    parser.add_argument("--deps_xml", help="Path to the XML file containing example dependencies.", required=True)
    parser.add_argument("--no_lib_resources", help="Do not add library resource dependencies.", action='store_true')
    args = parser.parse_args()
    
    file_path = str(args.project_dir) + '.settings/targets/xtensa/Release.bts'

    # all tflm examples' common dependencies
    base_entries_to_insert = [
        ["BuildSettings/BaseSettings/PreprocessorOptions/StringListMapOptions/StringListMapEntry", "Includes", "${workspace_loc}/../lib/aivoice/include"],
        
        ["BuildSettings/BaseSettings/PreprocessorOptions/KeyValueListMapOptions/KeyValueListMapEntry", "Defines", {"key": "USE_BINARY_RESOURCE", "value": "0"}],

        ["BuildSettings/BaseSettings/LinkerOptions/StringListMapOptions/StringListMapEntry", "Libraries", "aivoice"],
        ["BuildSettings/BaseSettings/LinkerOptions/StringListMapOptions/StringListMapEntry", "Libraries", "afe_kernel"],
        ["BuildSettings/BaseSettings/LinkerOptions/StringListMapOptions/StringListMapEntry", "Libraries", "kernel"],
        ["BuildSettings/BaseSettings/LinkerOptions/StringListMapOptions/StringListMapEntry", "Libraries", "afe_res_2mic50mm"],
        ["BuildSettings/BaseSettings/LinkerOptions/StringListMapOptions/StringListMapEntry", "Libraries", "vad_v7_200K"],
        ["BuildSettings/BaseSettings/LinkerOptions/StringListMapOptions/StringListMapEntry", "Libraries", "kws_xiaoqiangxiaoqiang_nihaoxiaoqiang_v4_300K"],
        ["BuildSettings/BaseSettings/LinkerOptions/StringListMapOptions/StringListMapEntry", "Libraries", "asr_cn_v8_2M"],
        ["BuildSettings/BaseSettings/LinkerOptions/StringListMapOptions/StringListMapEntry", "Libraries", "nnns_com_v7_35K"],
        ["BuildSettings/BaseSettings/LinkerOptions/StringListMapOptions/StringListMapEntry", "Libraries", "fst_cn_cmd_ac40"],
        ["BuildSettings/BaseSettings/LinkerOptions/StringListMapOptions/StringListMapEntry", "Libraries", "cJSON"],
        ["BuildSettings/BaseSettings/LinkerOptions/StringListMapOptions/StringListMapEntry", "Libraries", "tomlc99"],
        ["BuildSettings/BaseSettings/LinkerOptions/StringListMapOptions/StringListMapEntry", "Libraries", "tflite_micro"],
        ["BuildSettings/BaseSettings/LinkerOptions/StringListMapOptions/StringListMapEntry", "Libraries", "xa_nnlib"],
        ["BuildSettings/BaseSettings/LinkerOptions/StringListMapOptions/StringListMapEntry", "Libraries", "hifi5_dsp"],
        ["BuildSettings/BaseSettings/LinkerOptions/StringListMapOptions/StringListMapEntry", "Libraries", "aivoice_hal"],

        ["BuildSettings/BaseSettings/LinkerOptions/StringListMapOptions/StringListMapEntry", "LibrarySearchPath", "${workspace_loc}/../lib/aivoice/prebuilts/ameba_dsp/$(TARGET_CONFIG)"],
        ["BuildSettings/BaseSettings/LinkerOptions/StringListMapOptions/StringListMapEntry", "LibrarySearchPath", "${workspace_loc}/../lib/xa_nnlib/v2.3.0/bin/$(TARGET_CONFIG)/Release"],
        ["BuildSettings/BaseSettings/LinkerOptions/StringListMapOptions/StringListMapEntry", "LibrarySearchPath", "${workspace_loc}/../lib/lib_hifi5/v3.1.0/bin/$(TARGET_CONFIG)"],
    ]
    
    base_entries_no_lib_resouces_to_insert = [
        ["BuildSettings/BaseSettings/PreprocessorOptions/StringListMapOptions/StringListMapEntry", "Includes", "${workspace_loc}/../lib/aivoice/include"],
        ["BuildSettings/BaseSettings/PreprocessorOptions/KeyValueListMapOptions/KeyValueListMapEntry", "Defines", {"key": "USE_BINARY_RESOURCE", "value": "1"}],

        ["BuildSettings/BaseSettings/LinkerOptions/StringListMapOptions/StringListMapEntry", "Libraries", "aivoice"],
        ["BuildSettings/BaseSettings/LinkerOptions/StringListMapOptions/StringListMapEntry", "Libraries", "afe_kernel"],
        ["BuildSettings/BaseSettings/LinkerOptions/StringListMapOptions/StringListMapEntry", "Libraries", "kernel"],
        ["BuildSettings/BaseSettings/LinkerOptions/StringListMapOptions/StringListMapEntry", "Libraries", "cJSON"],
        ["BuildSettings/BaseSettings/LinkerOptions/StringListMapOptions/StringListMapEntry", "Libraries", "tomlc99"],
        ["BuildSettings/BaseSettings/LinkerOptions/StringListMapOptions/StringListMapEntry", "Libraries", "tflite_micro"],
        ["BuildSettings/BaseSettings/LinkerOptions/StringListMapOptions/StringListMapEntry", "Libraries", "xa_nnlib"],
        ["BuildSettings/BaseSettings/LinkerOptions/StringListMapOptions/StringListMapEntry", "Libraries", "hifi5_dsp"],
        ["BuildSettings/BaseSettings/LinkerOptions/StringListMapOptions/StringListMapEntry", "Libraries", "aivoice_hal"],

        ["BuildSettings/BaseSettings/LinkerOptions/StringListMapOptions/StringListMapEntry", "LibrarySearchPath", "${workspace_loc}/../lib/aivoice/prebuilts/ameba_dsp/$(TARGET_CONFIG)"],
        ["BuildSettings/BaseSettings/LinkerOptions/StringListMapOptions/StringListMapEntry", "LibrarySearchPath", "${workspace_loc}/../lib/xa_nnlib/v2.3.0/bin/$(TARGET_CONFIG)/Release"],
        ["BuildSettings/BaseSettings/LinkerOptions/StringListMapOptions/StringListMapEntry", "LibrarySearchPath", "${workspace_loc}/../lib/lib_hifi5/v3.1.0/bin/$(TARGET_CONFIG)"],
    ]
    
    if args.no_lib_resources:
        insert_base_entries_to_xml(file_path, base_entries_no_lib_resouces_to_insert)
        print("Skip adding library resource dependencies as --no_lib_resources is set.")
    else:
        insert_base_entries_to_xml(file_path, base_entries_to_insert)
    
    # add example-specific overridden settings
    add_overridden_settings_to_xml(file_path, args.deps_xml)

