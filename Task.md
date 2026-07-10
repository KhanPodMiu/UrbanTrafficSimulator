src/algorithms/
└── RoutingManager.cpp

src/simulation/
└── RouteOptimizer.cpp

include/algorithms/
└── RoutingManager.hpp

include/simulation/
└── RouteOptimizer.hpp

Task: Implement Strategy Pattern
Description

Create a routing abstraction layer using the Strategy Pattern.

Responsibilities

Define the PathFindingStrategy interface.

Support runtime algorithm switching.

Decouple routing logic from vehicles.

Enable future routing algorithm extensions.

 

Task: Implement Route Optimization
Description

Implement dynamic route recalculation based on traffic conditions.

Responsibilities

Detect route degradation caused by congestion.

Trigger route recomputation.

Select more efficient routes.

Support congestion-aware navigation.