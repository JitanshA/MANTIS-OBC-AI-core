import numpy as np
from h3_partitioning.h3_partition import generate_h3_cell_pixel_mapping
from algal_bloom.algal_bloom import predict_hab_for_cell
from tree_phenology.tree_phenology import compute_tree_features_for_cell
from utils import is_land_cell

# Load image (R, G, B, NIR)
image = np.random.randint(0, 255, (4, 2000, 2000), dtype=np.uint8)

# Metadata
top_left_lat = 45.0
top_left_lng = -75.0
pixel_size_m = 35.0
h3_res = 6

# Cloud filtering to be added here

# Partition image into H3 cells
h3_cells, cell_to_pixels = generate_h3_cell_pixel_mapping(
    top_left_lat=top_left_lat,
    top_left_lng=top_left_lng,
    h3_res=h3_res,
    image_shape=image.shape,
    pixel_size_m=pixel_size_m,
)

print("=== H3 Cell Results (First 5 Only) ===")
count = 0

for cell_id, pixel_list in cell_to_pixels.items():
    if count >= 5:
        break

    rows, cols = zip(*pixel_list)
    pixel_indices = (np.array(rows), np.array(cols))

    if is_land_cell(image, pixel_indices):
        tree_feats = compute_tree_features_for_cell(image, pixel_indices)
        print(f"H3 Cell: {cell_id} [LAND]")
        print(f"  → EVI:  {tree_feats['EVI']:.4f}")
        print(f"  → SAVI: {tree_feats['SAVI']:.4f}")
        print(f"  → GRVI: {tree_feats['GRVI']:.4f}")
    else:
        prob = predict_hab_for_cell(image, pixel_indices)
        print(f"H3 Cell: {cell_id} [WATER]")
        print(f"  → HAB Prediction: {prob:.4f}")

    count += 1
