#pragma once
#include "../core/pokemon.hpp"
#include "../data/game_data.hpp"
#include <string>

// Stack-based evolution system (like Super Auto Pets)
// Simplified version with helper function to reduce code duplication
class EvolutionSystem {
private:
  // Helper to get species by name
  static const SpeciesData *get_species(const std::string &name) {
    return GameData::getInstance().getSpecies(name);
  }

public:
  // Check if two Pokemon can be combined
  static bool can_combine(const Pokemon &mon1, const Pokemon &mon2) {
    // Must be same species
    if (mon1.name() != mon2.name())
      return false;

    // Must have an evolution available
    return get_evolution(mon1.species()) != nullptr;
  }

  // Get the evolution of a species (nullptr if no evolution)
  static const SpeciesData *get_evolution(const SpeciesData *species) {
    if (!species)
      return nullptr;

    std::string name = species->name;

    // Starter evolutions
    if (name == "Bulbasaur")
      return get_species("Ivysaur");
    if (name == "Ivysaur")
      return get_species("Venusaur");
    if (name == "Charmander")
      return get_species("Charmeleon");
    if (name == "Charmeleon")
      return get_species("Charizard");
    if (name == "Squirtle")
      return get_species("Wartortle");
    if (name == "Wartortle")
      return get_species("Blastoise");

    // Common Pokemon evolutions
    if (name == "Pidgey")
      return get_species("Pidgeotto");
    if (name == "Pidgeotto")
      return get_species("Pidgeot");
    if (name == "Rattata")
      return get_species("Raticate");
    if (name == "Spearow")
      return get_species("Fearow");
    if (name == "Ekans")
      return get_species("Arbok");
    if (name == "Pikachu")
      return get_species("Raichu");
    if (name == "Sandshrew")
      return get_species("Sandslash");
    if (name == "Nidoran-F")
      return get_species("Nidorina");
    if (name == "Nidorina")
      return get_species("Nidoqueen");
    if (name == "Nidoran-M")
      return get_species("Nidorino");
    if (name == "Nidorino")
      return get_species("Nidoking");
    if (name == "Clefairy")
      return get_species("Clefable");
    if (name == "Vulpix")
      return get_species("Ninetales");
    if (name == "Jigglypuff")
      return get_species("Wigglytuff");
    if (name == "Zubat")
      return get_species("Golbat");
    if (name == "Oddish")
      return get_species("Gloom");
    if (name == "Gloom")
      return get_species("Vileplume");
    if (name == "Paras")
      return get_species("Parasect");
    if (name == "Venonat")
      return get_species("Venomoth");
    if (name == "Diglett")
      return get_species("Dugtrio");
    if (name == "Meowth")
      return get_species("Persian");
    if (name == "Psyduck")
      return get_species("Golduck");
    if (name == "Mankey")
      return get_species("Primeape");
    if (name == "Growlithe")
      return get_species("Arcanine");
    if (name == "Poliwag")
      return get_species("Poliwhirl");
    if (name == "Poliwhirl")
      return get_species("Poliwrath");
    if (name == "Abra")
      return get_species("Kadabra");
    if (name == "Kadabra")
      return get_species("Alakazam");
    if (name == "Machop")
      return get_species("Machoke");
    if (name == "Machoke")
      return get_species("Machamp");
    if (name == "Bellsprout")
      return get_species("Weepinbell");
    if (name == "Weepinbell")
      return get_species("Victreebel");
    if (name == "Tentacool")
      return get_species("Tentacruel");
    if (name == "Geodude")
      return get_species("Graveler");
    if (name == "Graveler")
      return get_species("Golem");
    if (name == "Ponyta")
      return get_species("Rapidash");
    if (name == "Slowpoke")
      return get_species("Slowbro");
    if (name == "Magnemite")
      return get_species("Magneton");
    if (name == "Doduo")
      return get_species("Dodrio");
    if (name == "Seel")
      return get_species("Dewgong");
    if (name == "Grimer")
      return get_species("Muk");
    if (name == "Shellder")
      return get_species("Cloyster");
    if (name == "Gastly")
      return get_species("Haunter");
    if (name == "Haunter")
      return get_species("Gengar");
    if (name == "Drowzee")
      return get_species("Hypno");
    if (name == "Krabby")
      return get_species("Kingler");
    if (name == "Voltorb")
      return get_species("Electrode");
    if (name == "Exeggcute")
      return get_species("Exeggutor");
    if (name == "Cubone")
      return get_species("Marowak");
    if (name == "Koffing")
      return get_species("Weezing");
    if (name == "Rhyhorn")
      return get_species("Rhydon");
    if (name == "Horsea")
      return get_species("Seadra");
    if (name == "Goldeen")
      return get_species("Seaking");
    if (name == "Staryu")
      return get_species("Starmie");
    if (name == "Magikarp")
      return get_species("Gyarados");
    if (name == "Eevee")
      return get_species("Vaporeon"); // Default to Vaporeon
    if (name == "Omanyte")
      return get_species("Omastar");
    if (name == "Kabuto")
      return get_species("Kabutops");
    if (name == "Dratini")
      return get_species("Dragonair");
    if (name == "Dragonair")
      return get_species("Dragonite");

    return nullptr;
  }

  // Combine two Pokemon and evolve
  static Pokemon combine_and_evolve(const Pokemon &mon1, const Pokemon &mon2) {
    const SpeciesData *evolved_species = get_evolution(mon1.species());

    if (!evolved_species) {
      // Should not happen if can_combine was checked
      return mon1;
    }

    // Create evolved Pokemon at level 50
    Pokemon evolved(evolved_species->name, 50);

    return evolved;
  }
};
