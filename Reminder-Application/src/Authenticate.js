import React, { useEffect, useState } from "react";
import { StyleSheet, Text, TextInput, Button, Alert } from "react-native";
import { SafeAreaView } from "react-native-safe-area-context";
import { app } from "../firebase/firebaseConfig";
import { getAuth, createUserWithEmailAndPassword, signInWithEmailAndPassword, signOut } from "firebase/auth";

// Initialize Firebase
const auth = getAuth(app);


export default function Authentication({ navigation }) {
    const [email, setEmail] = useState("");
    const [password, setPassword] = useState("");

    // registers the user
    const handleSignUp = async () => {
        try {
            await createUserWithEmailAndPassword(auth, email, password);
        } catch (error) {
            Alert.alert("Sign Up Error", error.message);
        }
    };

    // logins the user
    const handleLogin = async () => {
        try {
            await signInWithEmailAndPassword(auth, email, password);
            navigation.navigate("Navigation");
        } catch (error) {
            Alert.alert("Login Error", error.message);
        }
    };

    return (
        <SafeAreaView style={styles.container}>
            <Text style={styles.header}>Login/Register</Text>

            <>
                <TextInput
                    placeholder="Email"
                    value={email}
                    onChangeText={setEmail}
                    style={styles.textInput}
                />
                <TextInput
                    placeholder="Password"
                    value={password}
                    onChangeText={setPassword}
                    secureTextEntry
                    style={styles.textInput}
                />

                <Button title="Sign Up" onPress={handleSignUp} />
                <Button title="Login" onPress={handleLogin} />
            </>
        </SafeAreaView >
    );
};

const styles = StyleSheet.create({
    container: {
        flex: 1,
        alignItems: "center",
        justifyContent: "center",
        gap: "3%"
    },
    header: {
        fontSize: 30,
        fontWeight: "bold"
    },
    textInput: {
        borderWidth: 1,
        padding: 8,
        marginVertical: 10,
        width: "90%"
    },
    button: {
        backgroundColor: "yellow"

    }


})