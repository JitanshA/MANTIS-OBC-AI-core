import numpy as np
from typing import Tuple


def is_land_cell(
    image: np.ndarray,
    pixel_indices: Tuple[np.ndarray, np.ndarray],
    ndvi_threshold: float = 0.2,
    land_fraction_threshold: float = 0.2
) -> bool:
    """
    Returns True if the H3 cell is considered land based on NDVI.

    Args:
        image (np.ndarray): 4-band image (R, G, B, NIR)
        pixel_indices (Tuple): (rows, cols) of the cell
        ndvi_threshold (float): NDVI value above which a pixel is considered land
        land_fraction_threshold (float): Minimum fraction of land pixels to count cell as land

    Returns:
        bool: True if H3 cell is land, False otherwise
    """
    rows, cols = pixel_indices
    red = image[0][rows, cols].astype(np.float32)
    nir = image[3][rows, cols].astype(np.float32)

    eps = 1e-6
    ndvi_vals = (nir - red) / (nir + red + eps)

    land_mask = ndvi_vals > ndvi_threshold
    land_fraction = np.sum(land_mask) / len(ndvi_vals)

    return land_fraction >= land_fraction_threshold
