# GEMSTAR UE_Vehicle_Feature

[![DLR-TS](https://img.shields.io/badge/developed%20by-DLR%20TS-orange.svg?style=flat&colorA=E1523D&colorB=007D8A)](https://www.dlr.de/en/ts/about-us/the-institute-of-transportation-systems)

3D vehicle models for the use in GEMSTAR. Maps names from scenario to asset, spawns and moves them accordingly to position updates.

![Mapping of vehicle in simulation to Asset](Documentation/Example_Vehicles.png)

## Input
- Vehicle Traffic Update Data from either [ue_sumo_feature](https://github.com/DLR-TS/ue_sumo_feature) or [ue_esmini_feature](https://github.com/DLR-TS/ue_esmini_feature).

# Getting started

First clone the GEMSTAR Interfaces plugin inside Plugins and the ue_visualizer_feature inside Plugins/GameFeatures.

Add the plugin to your Unreal project:
```sh
mkdir Plugins/GameFeatures
cd Plugins/GameFeatures

git clone https://github.com/DLR-TS/ue_vehicle_feature.git
```
or:
```sh
git submodule add https://github.com/DLR-TS/ue_vehicle_feature.git
```

# Configuration


```ini
[ue_vehicle_feature]
;Choose input from ue_esmini_feature or ue_sumo_feature
Input=[ue_esmini_feature, ue_sumo_feature]
;Path to vehicle data configuration
VehicleDataConfiguration=VehicleDataConfig.json
;SensorPlatform_X is a platform to mount sensors on. It is the name of the vehicle within in the scenario.
SensorPlatform_1=0
SensorPlatform_2=other
;Write a log with the mapping of Provider, ScenarioPlayerName, ScenarioPlayerType, VehicleAssetName, OsiObjectId
VehicleMappingLog=true
;Vehicle mapping file name
VehicleMappingFileName=GEMSTAR_vehicle_mapping.csv
;Clear vehicles with no position update during a simulation step
ClearAbandonedVehicles=true
```

The **Vehicle Definitions** defines the mapping of scenario names to asset names and can provide additional OSI data about vehicle types. An example is located in `Resources\Vehicle\VehicleDataConfig.json`

![Mapping of scenario name to vehicle in simulation](Documentation/Scenario-Vehicle_Mapping.png)

The Blueprint BP_VehicleManager contains the mappings from scenario vehicle name vehicle asset name.
The `Vehicle Definitions` load from `Vehicle Definitions Path`.

![Mapping of vehicle in simulation to Asset](Documentation/Vehicle-Asset_Mapping.png)

**Spawned Vehicles** Dynamic map of vehicles during the simulation\
**Available Vehicles** Map of all available vehicles in this plugin\
**Klick to update Vehicle Map** Tick the box to update the Available Vehicles Map. Use this after the addition of new models to check if the automatic discovery works.

## Adding new vehicles

Take a look at the [Tutorial](https://github.com/DLR-TS/gemstar/tree/main/Documentation/AddVehicle.md) in the GEMSTAR repository.

# Test

Testmaps are provided and located in Content/Test/
- with SUMO as a scenario player
- with esmini as a scenario player

## Data handling

This is the process when receiving new position data:

- The method VehicleDataUpdate_Implementation contains the parameters ControllerName, Name + Type or BoundingBox. Example: {esmini, Ego, VW_Golf_150PS, {x, y, z}}
  - If the name of the object is in SpawnedVehicles: Update object state
  - Else the object does not exist and needs to be spawned
    - Find vehicle type or vehicle name is found in the Vehicles Definitions datatable as name
    - Else find best matching bounding boxes defined by the Vehicle Definitions datatable
- The VehicleAsset name is then used to find the asset itself and be spawned
- The object is stored in the property pawnedVehicles of the VehicleManger actor

This approch allows for different vehicle definitons with the same asset, like different engine performances.
The datatable is easy to update and the mapping allows for easy customization of more complex simulation use cases.\
If the datasource does not issue traffic updates each frame use `ClearAbandonedVehicles=false` to keep the assets in the simulation.

---------------
**Contact:** opensource-ts@dlr.de

This project uses [esmini](https://github.com/esmini/esmini), which is licensed under the
Mozilla Public License 2.0.

This project uses the Open Simulation Interface ([OSI](https://github.com/OpenSimulationInterface/open-simulation-interface)), which is licensed under the
Mozilla Public License 2.0.
