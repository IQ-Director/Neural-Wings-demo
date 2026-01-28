<script setup>
defineProps({
  fullscreen: {
    default: null,
  },
  selectedResolution: {
    type: String,
    default: "",
  },
  isResolutionOpen: {
    type: Boolean,
    default: false,
  },
  targetFPS: {
    default: null,
  },
  resolutions: {
    type: Array,
    default: () => [],
  },
  toggleFullscreen: {
    type: Function,
    required: true,
  },
  toggleResolution: {
    type: Function,
    required: true,
  },
  chooseResolution: {
    type: Function,
    required: true,
  },
  changeFPS: {
    type: Function,
    required: true,
  },
  saveSettings: {
    type: Function,
    required: true,
  },
});
</script>

<template>
  <section class="menu-full settings-full pixel-font">
    <h1 class="splash-title">Game Settings</h1>
    <div class="settings-panel">
      <div class="form">
        <div class="row">
          <span>Fullscreen</span>
          <div class="settings-control">
            <button class="chip control-chip" @click="toggleFullscreen" :class="{ active: fullscreen === true }">
              {{ fullscreen === null ? "Not Set" : fullscreen ? "On" : "Off" }}
            </button>
          </div>
        </div>

        <div class="row">
          <span>Resolution</span>
          <div class="settings-control">
            <div class="dropdown" @click.stop>
              <button class="chip dropdown-toggle" @click="toggleResolution">
                {{ selectedResolution || "Select" }}
                <span class="caret"></span>
              </button>
              <div v-if="isResolutionOpen" class="dropdown-menu">
                <button v-for="res in resolutions" :key="res.value" class="dropdown-item"
                  @click="chooseResolution(res.value)">
                  {{ res.label }}
                </button>
              </div>
            </div>
          </div>
        </div>

        <div class="row">
          <span>FPS</span>
          <div class="settings-control">
            <div class="fps-control">
              <input class="fps-slider" type="range" :value="targetFPS === null ? '' : targetFPS"
                @input="changeFPS(Number($event.target.value))" @change="changeFPS(Number($event.target.value))"
                min="30" max="240" step="1" :disabled="targetFPS === null" />
              <span class="chip fps-value">
                {{ targetFPS === null ? "--" : targetFPS }} FPS
              </span>
            </div>
          </div>
        </div>
      </div>

      <div class="actions settings-actions">
        <button class="btn large primary" @click="saveSettings">Save Settings</button>
        <a class="btn large secondary" href="#/menu">Back to Menu</a>
      </div>
    </div>
  </section>
</template>
