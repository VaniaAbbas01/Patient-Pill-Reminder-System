import { StyleSheet, Text, Button } from 'react-native'
import { SafeAreaView } from "react-native-safe-area-context";
import React, { useEffect, useState } from 'react'
import { app } from "../firebase/firebaseConfig"
import { getDatabase, ref, onValue } from "firebase/database";
import { getAuth } from "firebase/auth"

const auth = getAuth(app);
const database = getDatabase(app);

export default function ViewHealthData({ navigation }) {
    // initialising state variables
    const [pulse, setPulse] = useState("");
    const [temperature, setTemperature] = useState("");
    const [user, setUser] = useState("");

    // loading uid upon page load
    useEffect(() => {

        const user = auth.currentUser

        if (user) setUser(user.uid);

    })

    // fetching pulse and temperature data when uid loads
    useEffect(() => {
        if (user) {
            // reading patient pulse
            const pulseRef = ref(database, `/sensorData/${user}/pulse`);
            const pulseListener = onValue(pulseRef, (snapshot) => {
                if (snapshot.exists()) {
                    setPulse(snapshot.val());
                }
            });

            // reading patient temperature
            const tempRef = ref(database, `/sensorData/${user}/temperature`);
            const tempListener = onValue(tempRef, (snapshot) => {
                if (snapshot.exists()) {
                    setTemperature(snapshot.val());
                }
            });
        }

    }, [user]);


    return (
        <SafeAreaView style={styles.container}>
            <Text style={styles.heading}>Paitent Vitals</Text>
            <Text style={styles.healthData}>Pulse Rate: {pulse}</Text>
            <Text style={styles.healthData}>Temperature: {temperature}</Text>
            <Button title="Back" onPress={() => { navigation.navigate("Navigation") }} />
        </SafeAreaView>
    )
}

const styles = StyleSheet.create({
    container: {
        flex: 1,
        justifyContent: "center",
        gap: "2%",
        alignItems: "center",
    },
    heading: {
        fontSize: 30,
        fontWeight: "bold"
    },
    healthData: {
        fontSize: 15,
        backgroundColor: "yellow",
        height: "10%",
        width: "50%",
        fontWeight: "bold",
        textAlign: "center",
        paddingTop: "7.5%"

    }
})