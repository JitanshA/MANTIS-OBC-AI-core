import numpy as np
from python.h3_partition import generate_h3_cell_pixel_mapping
from algal_bloom import predict_hab_for_cell
from utils import is_land_cell
from tree_phenology import compute_tree_features_for_cell

#TODO: Add L1-C like processing
def predict(image_path: str, center_lat:float, center_lng:float):
    image = np.random.randint(0, 255, (4, 2000, 2000), dtype=np.uint8)
    # TODO: Compute top left lat, lon from center coord
    # TODO: Cloud filtering to be added here
    
    _, cell_to_pixels = generate_h3_cell_pixel_mapping(center_lat,center_lng)
    for cell_id, pixel_list in cell_to_pixels.items():
        row, col = zip(*pixel_list)
        pixel_indices = (np.array(row), np.array(col))

        if is_land_cell(image, pixel_indices):
            tree_feats = compute_tree_features_for_cell(image, pixel_indices)
            return [str(cell_id), tree_feats["EVI"], tree_feats["SAVI"], tree_feats["GRVI"]]
        else:
            prob = predict_hab_for_cell(image, pixel_indices)
            return [str(cell_id), prob]
