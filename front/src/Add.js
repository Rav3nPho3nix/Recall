import React from 'react';
import './App.css';
import { useState, useEffect } from 'react';

export default function Add() {
    const [data, setData] = useState(null);

    useEffect(() => {
        async function fetchData() {
            try {
                const response = await fetch(`http://localhost:8888/add?name=toto&subject=TestSubject&number=0`);
                const json = await response.json();
                setData(json);
            } catch (error) {
                console.error("Erreur lors de la récupération des données :", error);
            }
        }

        fetchData();
    }, []);

    // Si le json n'a toujours pas ete traitee
    if (!data) {
        return (<div>Chargement de la réponse...</div>);
    }

    // Sinon
    return (
        <pre>
            {JSON.stringify(data, null, "\t")}
        </pre>
    );
}