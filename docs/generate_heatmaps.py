#!/usr/bin/env python3
"""
Generate heatmaps for Hong Kong and Singapore showing optimal locations
for technology companies based on opportunity and cost analysis.
"""

import matplotlib.pyplot as plt
import matplotlib.patches as patches
from matplotlib.colors import LinearSegmentedColormap
import numpy as np

def create_hong_kong_heatmap():
    """
    Create heatmap for Hong Kong showing tech company locations.
    
    Analysis based on 2026 data:
    - Central: High opportunity, very high cost (commercial hub)
    - Tsim Sha Tsui: High opportunity, high cost (data centers)
    - Tseung Kwan O: High opportunity, moderate cost (data center hub)
    - Wan Chai: Moderate opportunity, high cost
    - Causeway Bay: Moderate opportunity, high cost
    - Island East: Moderate opportunity, moderate-high cost
    - Kowloon Bay: Moderate opportunity, moderate cost (industrial tech)
    - Science Park: High opportunity, moderate cost (government support)
    - Cyberport: High opportunity, moderate cost (government support)
    """
    
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(16, 10))
    fig.suptitle('Hong Kong - Technology Company Location Analysis 2026', 
                 fontsize=16, fontweight='bold')
    
    # Define districts with coordinates (simplified layout)
    districts = {
        'Central': {'pos': (0.3, 0.5), 'opp': 9.5, 'cost': 9.8, 'size': 0.08},
        'Tsim Sha Tsui': {'pos': (0.25, 0.55), 'opp': 8.5, 'cost': 8.5, 'size': 0.07},
        'Tseung Kwan O': {'pos': (0.65, 0.3), 'opp': 9.0, 'cost': 6.0, 'size': 0.12},
        'Wan Chai': {'pos': (0.35, 0.48), 'opp': 7.0, 'cost': 8.5, 'size': 0.06},
        'Causeway Bay': {'pos': (0.4, 0.46), 'opp': 6.5, 'cost': 8.8, 'size': 0.05},
        'Island East': {'pos': (0.5, 0.42), 'opp': 7.5, 'cost': 7.5, 'size': 0.08},
        'Kowloon Bay': {'pos': (0.4, 0.6), 'opp': 7.0, 'cost': 6.5, 'size': 0.09},
        'Science Park': {'pos': (0.55, 0.7), 'opp': 8.8, 'cost': 6.5, 'size': 0.1},
        'Cyberport': {'pos': (0.2, 0.4), 'opp': 8.5, 'cost': 6.8, 'size': 0.09},
        'Kwun Tong': {'pos': (0.5, 0.55), 'opp': 6.8, 'cost': 6.0, 'size': 0.08},
    }
    
    # Opportunity heatmap
    create_district_heatmap(ax1, districts, 'opp', 'Opportunity Score',
                           'Opportunity: Data centers, AI/ML talent, government support')
    
    # Cost-effectiveness heatmap (inverse of cost)
    districts_cost = {k: {**v, 'cost_inv': 10 - v['cost']} for k, v in districts.items()}
    create_district_heatmap(ax2, districts_cost, 'cost_inv', 'Cost-Effectiveness Score',
                           'Cost-Effectiveness: Lower rent, competitive salaries')
    
    plt.tight_layout()
    plt.savefig('/home/runner/work/hektor/hektor/docs/hong_kong_tech_heatmap.png', 
                dpi=300, bbox_inches='tight')
    print("Hong Kong heatmap created: docs/hong_kong_tech_heatmap.png")
    plt.close()

def create_singapore_heatmap():
    """
    Create heatmap for Singapore showing tech company locations.
    
    Analysis based on 2026 data:
    - Core CBD: Very high opportunity, very high cost
    - Raffles Place: Very high opportunity, very high cost
    - Marina Bay: High opportunity, very high cost
    - One-North: Very high opportunity, moderate cost (Biopolis, Fusionopolis)
    - Changi Business Park: High opportunity, moderate cost
    - Jurong: Moderate opportunity, low-moderate cost (industrial)
    - Tanjong Pagar: High opportunity, high cost
    - Science Park: High opportunity, moderate-high cost
    - Woodlands: Moderate opportunity, low cost
    """
    
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(16, 10))
    fig.suptitle('Singapore - Technology Company Location Analysis 2026', 
                 fontsize=16, fontweight='bold')
    
    districts = {
        'Core CBD': {'pos': (0.5, 0.5), 'opp': 9.8, 'cost': 9.9, 'size': 0.08},
        'Raffles Place': {'pos': (0.52, 0.48), 'opp': 9.5, 'cost': 9.8, 'size': 0.06},
        'Marina Bay': {'pos': (0.54, 0.52), 'opp': 9.0, 'cost': 9.5, 'size': 0.07},
        'One-North': {'pos': (0.4, 0.45), 'opp': 9.5, 'cost': 6.5, 'size': 0.12},
        'Changi Business Park': {'pos': (0.75, 0.5), 'opp': 8.5, 'cost': 6.8, 'size': 0.11},
        'Tanjong Pagar': {'pos': (0.48, 0.46), 'opp': 8.8, 'cost': 8.5, 'size': 0.06},
        'Science Park': {'pos': (0.35, 0.55), 'opp': 8.8, 'cost': 7.5, 'size': 0.09},
        'Jurong': {'pos': (0.25, 0.5), 'opp': 7.0, 'cost': 5.5, 'size': 0.1},
        'Woodlands': {'pos': (0.5, 0.75), 'opp': 6.5, 'cost': 5.0, 'size': 0.08},
        'Paya Lebar': {'pos': (0.65, 0.48), 'opp': 7.5, 'cost': 7.0, 'size': 0.07},
    }
    
    # Opportunity heatmap
    create_district_heatmap(ax1, districts, 'opp', 'Opportunity Score',
                           'Opportunity: Tech ecosystem, talent, infrastructure')
    
    # Cost-effectiveness heatmap
    districts_cost = {k: {**v, 'cost_inv': 10 - v['cost']} for k, v in districts.items()}
    create_district_heatmap(ax2, districts_cost, 'cost_inv', 'Cost-Effectiveness Score',
                           'Cost-Effectiveness: Office rent, operational costs')
    
    plt.tight_layout()
    plt.savefig('/home/runner/work/hektor/hektor/docs/singapore_tech_heatmap.png', 
                dpi=300, bbox_inches='tight')
    print("Singapore heatmap created: docs/singapore_tech_heatmap.png")
    plt.close()

def create_district_heatmap(ax, districts, metric, title, subtitle):
    """Helper function to create a district-based heatmap."""
    
    # Create custom colormap (green = good, red = less favorable)
    colors = ['#d73027', '#fc8d59', '#fee08b', '#d9ef8b', '#91cf60', '#1a9850']
    n_bins = 100
    cmap = LinearSegmentedColormap.from_list('custom', colors, N=n_bins)
    
    # Create background
    ax.set_xlim(0, 1)
    ax.set_ylim(0, 1)
    ax.set_aspect('equal')
    
    # Plot districts as circles with colors based on metric
    for name, data in districts.items():
        x, y = data['pos']
        value = data[metric]
        size = data['size']
        
        # Draw circle
        circle = plt.Circle((x, y), size, color=cmap(value/10), alpha=0.7, ec='black', linewidth=2)
        ax.add_patch(circle)
        
        # Add label
        ax.text(x, y, f"{name}\n{value:.1f}/10", ha='center', va='center', 
               fontsize=8, fontweight='bold', color='white' if value > 5 else 'black',
               bbox=dict(boxstyle='round,pad=0.3', facecolor='black', alpha=0.3, edgecolor='none'))
    
    ax.set_title(f"{title}\n{subtitle}", fontsize=12, pad=10)
    ax.axis('off')
    
    # Add colorbar
    sm = plt.cm.ScalarMappable(cmap=cmap, norm=plt.Normalize(vmin=0, vmax=10))
    sm.set_array([])
    cbar = plt.colorbar(sm, ax=ax, orientation='horizontal', pad=0.05, fraction=0.046)
    cbar.set_label('Score (0-10)', fontsize=10)

if __name__ == '__main__':
    print("Generating Hong Kong technology heatmap...")
    create_hong_kong_heatmap()
    
    print("\nGenerating Singapore technology heatmap...")
    create_singapore_heatmap()
    
    print("\nHeatmaps generated successfully!")
