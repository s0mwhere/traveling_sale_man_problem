# Traveling Salesman Problem (TSP)

## 📌 Overview
The **Traveling Salesman Problem (TSP)** is a classical optimization problem in computer science and operations research. The goal is to determine the minimum-cost route for a traveler who must:

- Visit each city **exactly once**
- Return to the **starting city**
- Minimize total travel cost (distance, time, or weight)

The visiting order is not fixed, and all pairwise travel costs are known.  
TSP is an **NP-hard** problem and is commonly used to evaluate heuristic and approximation algorithms.

---

## 📚 References
- **Introduction to TSP**  
  https://youtu.be/GiDsjIBOVoA?si=pRDWfeiVPExQkT-I

- **A\* Search Algorithm**  
  https://youtu.be/88I6IidylGc?si=zk-0qYWB1miUDqmW

---

## 🧠 Solution Approach

This project solves the TSP using a **multi-stage hybrid algorithm** that combines shortest-path search, approximation, and local optimization.

### 1. Initial Graph Construction
- Construct a weighted graph with numbered nodes
- Identify a subset of **required nodes** that must be visited

### 2. Shortest Path Reduction (A\*)
- Apply the **A\*** algorithm to compute shortest paths between required nodes
- Build a reduced graph containing:
  - Only required nodes
  - Edge weights equal to shortest-path distances

### 3. Approximate Solution (Christofides Algorithm)
- Apply the **Christofides algorithm** on the reduced graph
- Produce a near-optimal Hamiltonian cycle
- Guarantees a solution within **1.5× optimal** for metric TSP

### 4. Local Optimization (3-Opt)
- Apply the **3-opt heuristic** to improve the tour
- Iteratively remove and reconnect edges
- Reduce total travel cost and improve solution quality

this readme.md is brought to you by chatgpt(the poor version)

## 🔁 Algorithm Pipeline
