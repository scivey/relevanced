from __future__ import print_function
from pprint import pprint
import random

MATH_TEST_URL = 'https://en.wikipedia.org/wiki/Matrix_multiplication'
POLITICAL_TEST_URL = 'https://en.wikipedia.org/wiki/Constitutional_republic'
MEDICAL_TEST_URL = 'https://en.wikipedia.org/wiki/Vitamin_B12_deficiency'

from .crawl import crawl_urls
from relevanced_client import Client

def get_max_key(scores):
    pairs = scores.items()
    pairs.sort(key=lambda x: x[1])
    return pairs[-1][0]

def get_client():
    from .main import get_client as main_get_client
    return main_get_client()

def get_centroid_url_pairs():
    return [
        ('math', MATH),
        ('politics', POLITICS),
        ('medicine', MEDICINE),
        ('prehistoric_fish', PREHISTORIC_FISH),
        ('coal_mining', COAL_MINING),
        ('volcanos', VOLCANOS),
        ('wine', WINE),
        ('art', ART),
        ('economics', ECONOMICS)
    ]


def load():
    client = get_client()

    centroid_url_pairs = get_centroid_url_pairs()

    for centroid_name, _ in centroid_url_pairs:
        client.create_centroid(centroid_name)

    for centroid_name, urls in centroid_url_pairs:
        data = crawl_urls(urls)
        for url, item in data.iteritems():
            client.create_document_with_id(url, item['text'])

    for centroid_name, urls in centroid_url_pairs:
        for url in urls:
            client.add_document_to_centroid(centroid_name, url)

def recompute():
    client = get_client()
    for name, _ in get_centroid_url_pairs():
        print('recomputing: %s' % name)
        client.recompute_centroid(name)

def test():
    client = get_client()
    centroids = [name for name, _ in get_centroid_url_pairs()]
    for name, urls in get_centroid_url_pairs():
        data = crawl_urls(urls)
        url = urls[random.randint(0, len(urls))]
        item = data[url]
        scores = client.multi_get_text_similarity(centroids, item['text'])
        best = get_max_key(scores)
        print('%s \t\t\t -> %s' % (url, best))

MATH = [
    'https://en.wikipedia.org/wiki/Linear_programming',
    'https://en.wikipedia.org/wiki/Algorithm',
    'https://en.wikipedia.org/wiki/Scalar_multiplication',
    'https://en.wikipedia.org/wiki/Mathematical_structure',
    'https://en.wikipedia.org/wiki/Dot_product',
    'https://en.wikipedia.org/wiki/Analysis_of_algorithms',
    'https://en.wikipedia.org/wiki/Linear_algebra',
    'https://en.wikipedia.org/wiki/Criss-cross_algorithm',
    'https://en.wikipedia.org/wiki/Support_vector_machine',
    'https://en.wikipedia.org/wiki/Mathematics',
    'https://en.wikipedia.org/wiki/Rational_number',
    'https://en.wikipedia.org/wiki/Fraction_(mathematics)',
    'https://en.wikipedia.org/wiki/Square_root_of_2',
    'https://en.wikipedia.org/wiki/Mathematical_optimization',
    'https://en.wikipedia.org/wiki/Optimization_problem',
    'https://en.wikipedia.org/wiki/Candidate_solution',
    'https://en.wikipedia.org/wiki/Search_algorithm'
]

POLITICS = [
    'https://en.wikipedia.org/wiki/Sovereign_state',
    'https://en.wikipedia.org/wiki/Executive_(government)',
    'https://en.wikipedia.org/wiki/Government',
    'https://en.wikipedia.org/wiki/Politics',
    'https://en.wikipedia.org/wiki/Federalism',
    'https://en.wikipedia.org/wiki/Separation_of_powers',
    'https://en.wikipedia.org/wiki/Autocracy',
    'https://en.wikipedia.org/wiki/Democracy',
    'https://en.wikipedia.org/wiki/Centralized_government',
    'https://en.wikipedia.org/wiki/Citizenship',
    'https://en.wikipedia.org/wiki/Public_interest',
    'https://en.wikipedia.org/wiki/Republic',
    'https://en.wikipedia.org/wiki/Political_philosophy'
]

MEDICINE = [
    'https://en.wikipedia.org/wiki/Cardiovascular_disease',
    'https://en.wikipedia.org/wiki/Antibiotics',
    'https://en.wikipedia.org/wiki/Aortic_aneurysm',
    'https://en.wikipedia.org/wiki/Medical_ultrasound',
    'https://en.wikipedia.org/wiki/Atherosclerosis',
    'https://en.wikipedia.org/wiki/Myocardial_infarction',
    'https://en.wikipedia.org/wiki/Coronary_artery_disease',
    'https://en.wikipedia.org/wiki/Diabetes_mellitus',
    'https://en.wikipedia.org/wiki/Metabolic_disorder',
    'https://en.wikipedia.org/wiki/Mitochondrial_disease',
    'https://en.wikipedia.org/wiki/Anemia'
]

PREHISTORIC_FISH = [
    'https://en.wikipedia.org/wiki/Arandaspis',
    'https://en.wikipedia.org/wiki/Astraspis',
    'https://en.wikipedia.org/wiki/Boreaspis',
    'https://en.wikipedia.org/wiki/Dartmuthia',
    'https://en.wikipedia.org/wiki/Doryaspis',
    'https://en.wikipedia.org/wiki/Drepanaspis',
    'https://en.wikipedia.org/wiki/Errivaspis',
    'https://en.wikipedia.org/wiki/Haikouichthys',
    'https://en.wikipedia.org/wiki/Hemicyclaspis',
    'https://en.wikipedia.org/wiki/Jamoytius',
    'https://en.wikipedia.org/wiki/Myllokunmingia',
    'https://en.wikipedia.org/wiki/Pikaia',
    'https://en.wikipedia.org/wiki/Pharyngolepis',
    'https://en.wikipedia.org/wiki/Promissum',
    'https://en.wikipedia.org/wiki/Pteraspis',
    'https://en.wikipedia.org/wiki/Thelodus',
    'https://en.wikipedia.org/wiki/Tremataspis',
    'https://en.wikipedia.org/wiki/Bothriolepis',
    'https://en.wikipedia.org/wiki/Coccosteus',
    'https://en.wikipedia.org/wiki/Ctenurella',
    'https://en.wikipedia.org/wiki/Dunkleosteus',
    'https://en.wikipedia.org/wiki/Gemuendina',
    'https://en.wikipedia.org/wiki/Groenlandaspis',
    'https://en.wikipedia.org/wiki/Materpiscis'
]

COAL_MINING = [
    'https://en.wikipedia.org/wiki/Coal_mining',
    'https://en.wikipedia.org/wiki/Coal',
    'https://en.wikipedia.org/wiki/Cement',
    'https://en.wikipedia.org/wiki/Iron_ore',
    'https://en.wikipedia.org/wiki/Open-pit_mining',
    'https://en.wikipedia.org/wiki/Longwall_mining',
    'https://en.wikipedia.org/wiki/Dragline_excavator',
    'https://en.wikipedia.org/wiki/History_of_coal_mining',
    'https://en.wikipedia.org/wiki/Drift_mine',
    'https://en.wikipedia.org/wiki/Surface_mining',
    'https://en.wikipedia.org/wiki/Geology',
    'https://en.wikipedia.org/wiki/Coal_preparation_plant',
    'https://en.wikipedia.org/wiki/Bituminous_coal',
    'https://en.wikipedia.org/wiki/Open_cast_mining',
    'https://en.wikipedia.org/wiki/Anthracite',
    'https://en.wikipedia.org/wiki/Dragline',
    'https://en.wikipedia.org/wiki/Mountaintop_removal',
    'https://en.wikipedia.org/wiki/Underground_mining_(soft_rock)',
    'https://en.wikipedia.org/wiki/Retreat_mining',
    'https://en.wikipedia.org/wiki/Conveyor_system',
    'https://en.wikipedia.org/wiki/Rock_blasting',
    'https://en.wikipedia.org/wiki/Hard_coal',
    'https://en.wikipedia.org/wiki/Brown_coal',
    'https://en.wikipedia.org/wiki/Lignite',
    'https://en.wikipedia.org/wiki/Coal_mining_in_the_United_States',
    'https://en.wikipedia.org/wiki/Coal_electricity',
    'https://en.wikipedia.org/wiki/Acid_mine_drainage',
    'https://en.wikipedia.org/wiki/Black_lung_disease',
    'https://en.wikipedia.org/wiki/Coal_preparation_plant',
    'https://en.wikipedia.org/wiki/Coal_slurry_impoundment',
    'https://en.wikipedia.org/wiki/Coal_train',
    'https://en.wikipedia.org/wiki/Coal-mining_region',
    'https://en.wikipedia.org/wiki/Problems_in_coal_mining',
    'https://en.wikipedia.org/wiki/World_Coal_Institute',
    'https://en.wikipedia.org/wiki/Coal_preparation_plant',
    'https://en.wikipedia.org/wiki/Coke_(fuel)',
    'https://en.wikipedia.org/wiki/Refined_coal',
    'https://en.wikipedia.org/wiki/Coal_town'
]

VOLCANOS = [
    'https://en.wikipedia.org/wiki/Basalt',
    'https://en.wikipedia.org/wiki/Caldera',
    'https://en.wikipedia.org/wiki/Cinder_cone',
    'https://en.wikipedia.org/wiki/Complex_volcano',
    'https://en.wikipedia.org/wiki/Cryovolcano',
    'https://en.wikipedia.org/wiki/Fissure_vent',
    'https://en.wikipedia.org/wiki/Geyser',
    'https://en.wikipedia.org/wiki/Hot_spring',
    'https://en.wikipedia.org/wiki/Hydrothermal_vent',
    'https://en.wikipedia.org/wiki/Lava_cone',
    'https://en.wikipedia.org/wiki/Lava_dome',
    'https://en.wikipedia.org/wiki/Mantle_(geology)',
    'https://en.wikipedia.org/wiki/Mud_pot',
    'https://en.wikipedia.org/wiki/Mud_volcano',
    'https://en.wikipedia.org/wiki/Prediction_of_volcanic_activity',
    'https://en.wikipedia.org/wiki/Pyroclastic_flow',
    'https://en.wikipedia.org/wiki/Pyroclastic_shield',
    'https://en.wikipedia.org/wiki/Rootless_cone',
    'https://en.wikipedia.org/wiki/Shield_volcano',
    'https://en.wikipedia.org/wiki/Somma_volcano',
    'https://en.wikipedia.org/wiki/Stratovolcano',
    'https://en.wikipedia.org/wiki/Submarine_volcano',
    'https://en.wikipedia.org/wiki/Supervolcano',
    'https://en.wikipedia.org/wiki/Types_of_volcanic_eruptions',
    'https://en.wikipedia.org/wiki/Vesuvius',
    'https://en.wikipedia.org/wiki/Volcanic_Explosivity_Index',
    'https://en.wikipedia.org/wiki/Volcanic_cone',
    'https://en.wikipedia.org/wiki/Volcanic_gas',
    'https://en.wikipedia.org/wiki/Volcanic_rock',
    'https://en.wikipedia.org/wiki/Volcanic_winter',
    'https://en.wikipedia.org/wiki/Volcano'
]

WINE = [
    'https://en.wikipedia.org/wiki/Glossary_of_wine_terms',
    'https://en.wikipedia.org/wiki/Glossary_of_winemaking_terms',
    'https://en.wikipedia.org/wiki/Health_effects_of_wine',
    'https://en.wikipedia.org/wiki/History_of_wine',
    'https://en.wikipedia.org/wiki/Muscat_(grape)',
    'https://en.wikipedia.org/wiki/Oak_(wine)',
    'https://en.wikipedia.org/wiki/Pinot_blanc',
    'https://en.wikipedia.org/wiki/Pinot_gris',
    'https://en.wikipedia.org/wiki/Pinot_noir',
    'https://en.wikipedia.org/wiki/Pressing_(wine)',
    'https://en.wikipedia.org/wiki/Red_wine',
    'https://en.wikipedia.org/wiki/Sweetness_of_wine',
    'https://en.wikipedia.org/wiki/Tuscan_wine',
    'https://en.wikipedia.org/wiki/Vine_training',
    'https://en.wikipedia.org/wiki/Vineyard',
    'https://en.wikipedia.org/wiki/White_wine',
    'https://en.wikipedia.org/wiki/Willamette_Valley_(wine)',
    'https://en.wikipedia.org/wiki/Wine',
    'https://en.wikipedia.org/wiki/Wine_and_health',
    'https://en.wikipedia.org/wiki/Wine_barrel',
    'https://en.wikipedia.org/wiki/Wine_bottle',
    'https://en.wikipedia.org/wiki/Wine_cellar',
    'https://en.wikipedia.org/wiki/Wine_tasting',
    'https://en.wikipedia.org/wiki/Winemaking',
    'https://en.wikipedia.org/wiki/Zinfandel'
]

ART = [
    'https://en.wikipedia.org/wiki/Art',
    'https://en.wikipedia.org/wiki/Art_movement',
    'https://en.wikipedia.org/wiki/Avant-garde',
    'https://en.wikipedia.org/wiki/Ayn_Rand',
    'https://en.wikipedia.org/wiki/Bad_Painting',
    'https://en.wikipedia.org/wiki/Baroque',
    'https://en.wikipedia.org/wiki/Bauhaus',
    'https://en.wikipedia.org/wiki/Bay_Area_Figurative_Movement',
    'https://en.wikipedia.org/wiki/Brush_painting',
    'https://en.wikipedia.org/wiki/Buon_fresco',
    'https://en.wikipedia.org/wiki/Collage',
    'https://en.wikipedia.org/wiki/Composition_(visual_arts)',
    'https://en.wikipedia.org/wiki/Contemporary_Art',
    'https://en.wikipedia.org/wiki/Cubism',
    'https://en.wikipedia.org/wiki/Digital_Art',
    'https://en.wikipedia.org/wiki/Digital_painting',
    'https://en.wikipedia.org/wiki/Drawing',
    'https://en.wikipedia.org/wiki/Drying_oil',
    'https://en.wikipedia.org/wiki/Encaustic_painting',
    'https://en.wikipedia.org/wiki/Expressionism',
    'https://en.wikipedia.org/wiki/Figurative_art',
    'https://en.wikipedia.org/wiki/Figure_painting',
    'https://en.wikipedia.org/wiki/Fixative_(drawing)',
    'https://en.wikipedia.org/wiki/Fresco',
    'https://en.wikipedia.org/wiki/Geometric_abstraction',
    'https://en.wikipedia.org/wiki/Gestural_painting',
    'https://en.wikipedia.org/wiki/Graffiti_Art',
    'https://en.wikipedia.org/wiki/Hyperrealism_(painting)',
    'https://en.wikipedia.org/wiki/Impressionism',
    'https://en.wikipedia.org/wiki/Ink_and_wash_painting',
    'https://en.wikipedia.org/wiki/Landscape_painting',
    'https://en.wikipedia.org/wiki/Modern_Art',
    'https://en.wikipedia.org/wiki/Mural_painting',
    'https://en.wikipedia.org/wiki/Neo-expressionism',
    'https://en.wikipedia.org/wiki/Oil_painting',
    'https://en.wikipedia.org/wiki/Style_(visual_arts)',
    'https://en.wikipedia.org/wiki/Stylization',
    'https://en.wikipedia.org/wiki/Surrealism',
    'https://en.wikipedia.org/wiki/Visual_arts'
]

ECONOMICS = [
    'https://en.wikipedia.org/wiki/Marginal_cost',
    'https://en.wikipedia.org/wiki/Marginal_revenue',
    'https://en.wikipedia.org/wiki/Marginal_utility',
    'https://en.wikipedia.org/wiki/Market_(economics)',
    'https://en.wikipedia.org/wiki/Market_economy',
    'https://en.wikipedia.org/wiki/Market_equilibrium',
    'https://en.wikipedia.org/wiki/Market_failure',
    'https://en.wikipedia.org/wiki/Market_power',
    'https://en.wikipedia.org/wiki/Market_structure',
    'https://en.wikipedia.org/wiki/Market_system',
    'https://en.wikipedia.org/wiki/Marketplace',
    'https://en.wikipedia.org/wiki/Markets',
    'https://en.wikipedia.org/wiki/Marxian_economics',
    'https://en.wikipedia.org/wiki/Mathematical_economics',
    'https://en.wikipedia.org/wiki/Measures_of_national_income_and_output',
    'https://en.wikipedia.org/wiki/Mercantilism',
    'https://en.wikipedia.org/wiki/Microeconomics',
    'https://en.wikipedia.org/wiki/Mixed_economies',
    'https://en.wikipedia.org/wiki/Model_(economics)',
    'https://en.wikipedia.org/wiki/Monetary_economics',
    'https://en.wikipedia.org/wiki/Monetary_policy',
    'https://en.wikipedia.org/wiki/Monetary_theory',
    'https://en.wikipedia.org/wiki/Money',
    'https://en.wikipedia.org/wiki/Money_supply',
    'https://en.wikipedia.org/wiki/Monopolistic_competition',
    'https://en.wikipedia.org/wiki/Neoclassical_economics',
    'https://en.wikipedia.org/wiki/New-Keynesian_economics',
    'https://en.wikipedia.org/wiki/Nominal_value',
    'https://en.wikipedia.org/wiki/Opportunity_cost',
    'https://en.wikipedia.org/wiki/Outline_of_economics',
    'https://en.wikipedia.org/wiki/Output_(economics)',
    'https://en.wikipedia.org/wiki/Pareto_efficiency',
    'https://en.wikipedia.org/wiki/Per_capita',
    'https://en.wikipedia.org/wiki/Perfect_competition',
    'https://en.wikipedia.org/wiki/Perfect_information',
    'https://en.wikipedia.org/wiki/Permanent_income_hypothesis',
    'https://en.wikipedia.org/wiki/Planned_economy',
    'https://en.wikipedia.org/wiki/Positive_relationship',
    'https://en.wikipedia.org/wiki/Post-Keynesian_economics',
    'https://en.wikipedia.org/wiki/Potential_output',
    'https://en.wikipedia.org/wiki/Poverty',
    'https://en.wikipedia.org/wiki/Preference_(economics)',
    'https://en.wikipedia.org/wiki/Price',
    'https://en.wikipedia.org/wiki/Price_elasticity_of_supply',
    'https://en.wikipedia.org/wiki/Price_level',
    'https://en.wikipedia.org/wiki/Price_stickiness',
    'https://en.wikipedia.org/wiki/Price_system',
    'https://en.wikipedia.org/wiki/Prices_and_quantities',
    'https://en.wikipedia.org/wiki/Profit_(economics)',
    'https://en.wikipedia.org/wiki/Profit_maximization',
    'https://en.wikipedia.org/wiki/Purchasing_power',
    'https://en.wikipedia.org/wiki/Qualitative_economics',
    'https://en.wikipedia.org/wiki/Quantity_theory_of_money',
    'https://en.wikipedia.org/wiki/Rate_of_profit',
    'https://en.wikipedia.org/wiki/Rational_choice_theory',
    'https://en.wikipedia.org/wiki/Rational_expectations',
    'https://en.wikipedia.org/wiki/Real_GDP',
    'https://en.wikipedia.org/wiki/Real_business_cycle_theory',
    'https://en.wikipedia.org/wiki/Real_versus_nominal_value_(economics)',
    'https://en.wikipedia.org/wiki/Recession',
    'https://en.wikipedia.org/wiki/Regulatory_economics',
    'https://en.wikipedia.org/wiki/Schools_of_economics',
    'https://en.wikipedia.org/wiki/Service_(economics)',
    'https://en.wikipedia.org/wiki/Sticky_(economics)',
    'https://en.wikipedia.org/wiki/Stock_and_flow',
    'https://en.wikipedia.org/wiki/Stockholm_school_(economics)',
    'https://en.wikipedia.org/wiki/Structuralist_economics',
    'https://en.wikipedia.org/wiki/Substitution_effect',
    'https://en.wikipedia.org/wiki/Sunk_costs',
    'https://en.wikipedia.org/wiki/Supply-side_economics',
    'https://en.wikipedia.org/wiki/Supply_and_demand',
    'https://en.wikipedia.org/wiki/Tariff',
    'https://en.wikipedia.org/wiki/Theory_of_the_firm',
    'https://en.wikipedia.org/wiki/Transaction_cost',
    'https://en.wikipedia.org/wiki/Unemployment_rate',
    'https://en.wikipedia.org/wiki/Utility',
    'https://en.wikipedia.org/wiki/Value_and_Capital',
    'https://en.wikipedia.org/wiki/Wealth_(economics)'
]

if __name__ == '__main__':
    main()