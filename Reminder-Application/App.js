import React from "react";
import { NavigationContainer } from "@react-navigation/native";
import { createNativeStackNavigator } from "@react-navigation/native-stack";

const Stack = createNativeStackNavigator();

import Navigation from "./src/Navigation";
import CreateReminder from "./src/CreateReminder";
import ViewHealthData from "./src/ViewHealthData";
import Authentication from "./src/Authenticate";

const PillReminderApp = () => {
  return (
    <NavigationContainer>
      <Stack.Navigator screenOptions={{ headerShown: false }}>
        <Stack.Screen name="Navigation" component={Navigation} />
        <Stack.Screen name="CreateReminder" component={CreateReminder} />
        <Stack.Screen name="ViewHealthData" component={ViewHealthData} />
        <Stack.Screen name="Authentication" component={Authentication} />
      </Stack.Navigator>
    </NavigationContainer>

  );
};

export default PillReminderApp;
