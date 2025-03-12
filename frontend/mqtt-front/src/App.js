import React from "react";
import { BrowserRouter as Router, Routes, Route } from "react-router-dom";
import "./App.css";
import RegisterDevice from "./components/RegisterDevice";
import DeviceList from "./components/DeviceList";
import Dashboard from "./components/Dashboard"; // Importando o Dashboard

export default function App() {
  return (
    <Router>
      <Routes>
        {/* Página inicial com Registro e Lista de Dispositivos */}
        <Route
          path="/"
          element={
            <div class="container">
              <RegisterDevice />
              <DeviceList />
            </div>
          }
        />
        
        {/* Dashboard de um dispositivo específico */}
        <Route path="/dashboard/:device_id" element={<Dashboard />} />
      </Routes>
    </Router>
  );
}
