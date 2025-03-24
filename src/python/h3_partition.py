import numpy as np
import h3
from typing import Dict, List, Set, Tuple
import math

def compute_top_left_lat_lon(center_lat, center_lon,
                             width_m=2048, height_m=2448):
    meters_per_degree_lat = 110574.0
    meters_per_degree_lon = 111320.0 * math.cos(math.radians(center_lat))
    half_height = height_m / 2.0
    half_width  = width_m  / 2.0
    delta_lat = half_height / meters_per_degree_lat
    delta_lon = half_width  / meters_per_degree_lon
    top_left_lat = center_lat + delta_lat  # going 'north' increases latitude
    top_left_lon = center_lon - delta_lon  # going 'west' decreases longitude
    return top_left_lat, top_left_lon

def generate_h3_cell_pixel_mapping(
    center_lat: float,
    center_lng: float,
    h3_res: int = 6,
    image_shape: Tuple[int, int, int] = (4, 2048, 2448),
    pixel_size_m: float = 35.0,
    ) -> Tuple[Set[str], Dict[str, List[Tuple[int, int]]]]:
    
    # TODO: Confirm which side of image is 2448 vs 2048 for top left calc
    
    top_left_lat, top_left_lng = compute_top_left_lat_lon(center_lat, center_lng)

    _, height, width = image_shape
    pixel_size_deg = pixel_size_m / 111320.0  # meters to degrees (approximate)

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
