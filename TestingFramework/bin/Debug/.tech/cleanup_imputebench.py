import os
import shutil

def get_immediate_subdirectories(a_dir):
    return [name for name in os.listdir(a_dir)
            if os.path.isdir(os.path.join(a_dir, name))]

for dir_scen in get_immediate_subdirectories("."):
    dir_scen_full = dir_scen
    print(dir_scen_full);
    for dir_dataset in get_immediate_subdirectories(dir_scen_full):
        dir_dataset_full = dir_scen_full + "/" + dir_dataset;
        print(dir_dataset_full);
        dir_data = dir_dataset_full + "/recovery/values/";
        if os.path.isdir(dir_data):
            print("\tfound! " + dir_data);
            shutil.rmtree(dir_data, ignore_errors=True);
            print("\tdirectory deleted" );
        else:
            print("\t directory not found. moving on..." );
