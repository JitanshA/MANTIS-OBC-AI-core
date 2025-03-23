import numpy as np
from typing import Tuple, Dict, Any


def compute_tree_features_for_cell(
    image: np.ndarray,
    pixel_indices: Tuple[np.ndarray, np.ndarray],
    ndvi_threshold: float = 0.1
) -> Dict[str, float]:
    """
    Computes vegetation-related features (excluding NDVI) for a given H3 cell,
    assuming only forested/land cells are passed.

    Band order in image:
    - Red   = image[0]
    - Green = image[1]
    - Blue  = image[2]
    - NIR   = image[3]

    Args:
        image (np.ndarray): 4-band satellite image (Red, Green, Blue, NIR)
        pixel_indices (Tuple[np.ndarray, np.ndarray]): (rows, cols) for the cell
        ndvi_threshold (float): Minimum NDVI for considering as "land/vegetated"

    Returns:
        Dict[str, float]: Tree-related features per cell
    """
    rows, cols = pixel_indices
    red = image[0][rows, cols].astype(np.float32) / 255.0
    green = image[1][rows, cols].astype(np.float32) / 255.0
    blue = image[2][rows, cols].astype(np.float32) / 255.0
    nir = image[3][rows, cols].astype(np.float32) / 255.0

    eps = 1e-6

    # Compute NDVI for masking land
    ndvi_vals = (nir - red) / (nir + red + eps)
    land_mask = ndvi_vals > ndvi_threshold

    if np.sum(land_mask) == 0:
        return {
            "EVI": 0.0,
            "SAVI": 0.0,
            "GRVI": 0.0,
        }

    red_l = red[land_mask]
    green_l = green[land_mask]
    blue_l = blue[land_mask]
    nir_l = nir[land_mask]

    # EVI: Enhanced Vegetation Index
    # EVI = 2.5 * (NIR - Red) / (NIR + 6*Red - 7.5*Blue + 1)
    evi = 2.5 * (nir_l - red_l) / (nir_l + 6 * red_l - 7.5 * blue_l + 1 + eps)

    # SAVI: Soil-Adjusted Vegetation Index (L = 0.5)
    savi = ((nir_l - red_l) / (nir_l + red_l + 0.5 + eps)) * 1.5

    # GRVI: Green-Red Vegetation Index = (Green - Red) / (Green + Red)
    grvi = (green_l - red_l) / (green_l + red_l + eps)

    return {
        "EVI": float(np.mean(evi)),
        "SAVI": float(np.mean(savi)),
        "GRVI": float(np.mean(grvi)),
    }
