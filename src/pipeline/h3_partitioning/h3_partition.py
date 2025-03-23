import numpy as np
import h3
from typing import Dict, List, Set, Tuple


def generate_h3_cell_pixel_mapping(
    top_left_lat: float,
    top_left_lng: float,
    h3_res: int = 6,
    image_shape: Tuple[int, int, int] = (4, 2000, 2000),
    pixel_size_m: float = 35.0,
) -> Tuple[Set[str], Dict[str, List[Tuple[int, int]]]]:
    """
    Generates H3 cells covering a satellite image and maps each cell to the pixel indices it contains.

    Args:
        top_left_lat (float): Latitude of the top-left corner of the image
        top_left_lng (float): Longitude of the top-left corner of the image
        h3_res (int): H3 resolution level (default: 6 translating to edge length 1200 m, average area 2.56 km^2)
        image_shape (Tuple[int, int, int]): Shape of the image as (bands, height, width)
        pixel_size_m (float): Pixel size in meters (assumes square pixels)

    Returns:
        Tuple[
            Set[str]: Unique H3 cell IDs,
            Dict[str, List[Tuple[int, int]]]: Mapping from H3 cell to list of (row, col) pixel indices
        ]
    """
    _, height, width = image_shape
    pixel_size_deg = pixel_size_m / 111320  # meters to degrees (approximate)

    lat_grid = top_left_lat - np.arange(height) * pixel_size_deg
    lng_grid = top_left_lng + np.arange(width) * pixel_size_deg

    lat_mesh, lng_mesh = np.meshgrid(lat_grid, lng_grid, indexing='ij')

    cell_to_pixels: Dict[str, List[Tuple[int, int]]] = {}

    for row in range(height):
        for col in range(width):
            lat = lat_mesh[row, col]
            lng = lng_mesh[row, col]
            cell_id = h3.latlng_to_cell(lat, lng, h3_res)

            if cell_id not in cell_to_pixels:
                cell_to_pixels[cell_id] = []

            cell_to_pixels[cell_id].append((row, col))

    h3_cells = set(cell_to_pixels.keys())

    return h3_cells, cell_to_pixels
